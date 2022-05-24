#include <Windows.h>
#include <stdio.h>

typedef struct task_s {
	void *p_data;
	void(*task_proc)(void *p);
} task_t;

typedef struct threadpool_s {
	int stack_size;
	int current_pos;
	task_t *p_tasks;
} threadpool_t;

CONDITION_VARIABLE cnd;
CRITICAL_SECTION cs;
threadpool_t thrdpool;

DWORD WINAPI thread_proc(LPVOID param)
{
	threadpool_t *p_threadpool = (threadpool_t *)param;
	while (1) {
		task_t task;
		EnterCriticalSection(&cs);

		//check empty stack
		if (!p_threadpool->current_pos) {
			SleepConditionVariableCS(&cnd, &cs, INFINITE);
		}

		task = p_threadpool->p_tasks[p_threadpool->current_pos];

		if(p_threadpool->current_pos > 0)
			p_threadpool->current_pos--;
		else {
			printf("p_threadpool->current_pos WAS 0 !!!!\n");
		}

		LeaveCriticalSection(&cs);

		//asynchronize this
		task.task_proc(task.p_data);
	}
	return 0;
}

void task_proc_work(void *p)
{
	int active = thrdpool.current_pos;
	int c = 0;
	for (int i = 0; i < 100000; i++)
		for (int k = 0; k < 10000; k++)
			c = !c;

	printf("result %d  Task finished!  Active tasks: %d\n", c, active);
}

int main()
{
	InitializeConditionVariable(&cnd);
	InitializeCriticalSection(&cs);
	
	thrdpool.current_pos = 0;
	thrdpool.stack_size = 1024;
	thrdpool.p_tasks = (task_t *)malloc(sizeof(task_t) * thrdpool.stack_size);

	SYSTEM_INFO sysinf;
	GetSystemInfo(&sysinf);

	HANDLE h_threads[48];
	for (int i = 0; i < sysinf.dwNumberOfProcessors; i++)
		if ((h_threads[i] = CreateThread(0, 0, thread_proc, &thrdpool, 0, 0)))
			SetThreadAffinityMask(h_threads[i], (1 << i));

	printf("\n");
	while (1) {
		getchar();
		int j = 0;
		for (int i = 0; i < 20; i++) {
			if (thrdpool.current_pos < thrdpool.stack_size) {
				thrdpool.current_pos++;
				thrdpool.p_tasks[thrdpool.current_pos].p_data = 0;
				thrdpool.p_tasks[thrdpool.current_pos].task_proc = task_proc_work;
				j++;
				WakeConditionVariable(&cnd); //wake thread to running task
			}
		}
		printf("Added %d tasks!\n", j);
	}

	WaitForMultipleObjects(sysinf.dwNumberOfProcessors, h_threads, TRUE, INFINITE);
	DeleteCriticalSection(&cs);
	//free(thrdpool.p_tasks);
	return 0;
}