#include "threads.h"

int get_logical_processors_count()
{
	SYSTEM_INFO sysinf;
	GetSystemInfo(&sysinf);
	return (int)sysinf.dwNumberOfProcessors;
}

int get_physical_processors_count()
{
	int this_function_is_not_implemented = 0;
	assert(this_function_is_not_implemented);
	return this_function_is_not_implemented;
}

int thread_create(thread_t *p_threadhandle, thread_proc proc, void *arg)
{
	p_threadhandle->handle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)proc, arg, NULL, NULL);
	return (p_threadhandle->handle != NULL);
}

int thread_resume(thread_t *p_threadhandle)
{
	ResumeThread(p_threadhandle->handle);
	return 1;
}

int thread_suspend(thread_t *p_threadhandle)
{
	SuspendThread(p_threadhandle->handle);
	return 1;
}

int thread_setaffinity(thread_t *p_threadhandle, void *p_data)
{
	SetThreadAffinityMask(p_threadhandle->handle, (DWORD_PTR)p_data);
	return 1;
}

int thread_join(thread_t *p_threadhandle)
{
	WaitForSingleObject(p_threadhandle->handle, INFINITE);
	return 1;
}

int thread_detach(thread_t *p_threadhandle)
{
	CloseHandle(p_threadhandle->handle);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////
void mutex_init(mtx_t *p_mutex)
{
	InitializeCriticalSection(&p_mutex->cs);
}
 
void mutex_lock(mtx_t *p_mutex)
{
	EnterCriticalSection(&p_mutex->cs);
}

void mutex_unlock(mtx_t *p_mutex)
{
	LeaveCriticalSection(&p_mutex->cs);
}

void mutex_deinit(mtx_t *p_mutex)
{
	DeleteCriticalSection(&p_mutex->cs);
}

///////////////////////////////////////////////////////////////////////////////////
void cnd_init(cnd_t *p_condvar)
{
	InitializeConditionVariable(&p_condvar->cv);
}

void cnd_wake_one(cnd_t *p_condvar)
{
	WakeConditionVariable(&p_condvar->cv);
}

void cnd_wake_all(cnd_t *p_condvar)
{
	WakeAllConditionVariable(&p_condvar->cv);
}

void cnd_wait(cnd_t *p_condvar, mtx_t *p_mutex)
{
	SleepConditionVariableCS(&p_condvar->cv, &p_mutex->cs, INFINITE);
}

///////////////////////////////////////////////////////////////////////////////////
void *worker_thread_proc(void *p_arg)
{
	threadpool_t *p_tp = (threadpool_t *)p_arg;
	while (p_tp->state == TPSTATUS_RUNNING) {
		tptask_t task;
		mutex_lock(&p_tp->mutex);

		//if stack is empty
		if (!p_tp->current_task)
			cnd_wait(&p_tp->cvtask, &p_tp->mutex); //freeze threads

		task = p_tp->p_tasks[p_tp->current_task];
		if (p_tp->current_task > 0)
			p_tp->current_task--;

		// update tasks statistics
		p_tp->statistic.active_threads = p_tp->num_of_threads;
		p_tp->statistic.task_sequence = p_tp->current_task;
		p_tp->statistic.task_stack_size = p_tp->tasks_capacity;
		mutex_unlock(&p_tp->mutex);

		task.taskproc(&p_tp->statistic, task.p_arg); // run asynch
	}
	return NULL;
}

int threadpool_init(threadpool_t *p_tp, int tasks_limit)
{
	p_tp->current_task = 0;
	p_tp->tasks_capacity = tasks_limit;
	mutex_init(&p_tp->mutex);
	cnd_init(&p_tp->cvtask);
	cnd_init(&p_tp->cvfinish);
	p_tp->p_tasks = (tptask_t *)calloc(p_tp->tasks_capacity, sizeof(tptask_t)); /* alloc memory for tasks */
	if (p_tp->p_tasks) {
		p_tp->num_of_threads = get_logical_processors_count();
		if (p_tp->num_of_threads) {
			p_tp->p_threads = (thread_t *)calloc(p_tp->num_of_threads, sizeof(tptask_t)); /* alloc memory for threads handles */
			if (p_tp->p_threads) {
				for (int i = 0; i < p_tp->num_of_threads; i++) {
					//thread_create(&p_tp->p_threads[i], worker_thread_proc, p_tp);
					if (thread_create(&p_tp->p_threads[i], worker_thread_proc, p_tp)) {
						//thread_setaffinity(&p_tp->p_threads[i], (void *)(1 << i));
						SetThreadAffinityMask(p_tp->p_threads[i].handle, (1 << i));
					}
				}
				p_tp->state = TPSTATUS_RUNNING; /* ok. set running state */
				return 0;
			}
			assert(p_tp->p_threads);
			return 3;
		}
		assert(p_tp->num_of_threads);
		return 2;
	}
	assert(p_tp->p_tasks);
	return 1;
}

int threadpool_add_task(threadpool_t *p_tp, int priority, TASKPROC taskproc, void *p_arg)
{
	mutex_lock(&p_tp->mutex);
	if (p_tp->current_task < p_tp->tasks_capacity) {
		p_tp->current_task++; //TODO: KOSTYL!
		tptask_t *p_task = &p_tp->p_tasks[p_tp->current_task];
		p_task->priority = priority;
		p_task->taskproc = taskproc;
		p_task->p_arg = p_arg;
		cnd_wake_one(&p_tp->cvtask);

		printf("task %d | proc 0x%x | arg 0x%x\n", p_tp->current_task, p_task->taskproc, p_task->p_arg);
	}
	mutex_unlock(&p_tp->mutex);
	return 0;
}

int threadpool_suspend(threadpool_t *p_tp)
{
	for (int i = 0; i < p_tp->num_of_threads; i++)
		thread_suspend(&p_tp->p_threads[i]);

	return 1;
}

int threadpool_resume(threadpool_t *p_tp)
{
	for (int i = 0; i < p_tp->num_of_threads; i++)
		thread_resume(&p_tp->p_threads[i]);

	return 1;
}

int threadpool_set_state(threadpool_t *p_tp, int _state)
{
	return InterlockedExchange(&p_tp->state, _state);
}

int threadpool_join(threadpool_t *p_tp)
{
	for (int i = 0; i < p_tp->num_of_threads; i++)
		thread_join(&p_tp->p_threads[i]);

	return 1;
}

int threadpool_free(threadpool_t *p_tp)
{
	threadpool_suspend(p_tp);
	if (p_tp->p_tasks)
		free(p_tp->p_tasks);

	if (p_tp->p_threads) {
		for (int i = 0; i < p_tp->num_of_threads; i++)
			thread_detach(&p_tp->p_threads[i]);

		free(p_tp->p_threads);
	}
	mutex_deinit(&p_tp->mutex);
	return 1;
}