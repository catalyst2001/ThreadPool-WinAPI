#pragma once
#include <Windows.h>
#include <stdbool.h>
#include <assert.h>

// 
// Threads api
// 
typedef struct thread_s {
	HANDLE handle;
} thread_t;

int get_logical_processors_count();
int get_physical_processors_count();

typedef void *(*thread_proc)(void *arg);
int thread_create(thread_t *p_threadhandle, thread_proc proc, void *arg);
int thread_resume(thread_t *p_threadhandle);
int thread_suspend(thread_t *p_threadhandle);
int thread_setaffinity(thread_t *p_threadhandle, void *p_data);
int thread_join(thread_t *p_threadhandle);
int thread_detach(thread_t *p_threadhandle);

// 
// mutex
// 
typedef struct mtx_s {
	CRITICAL_SECTION cs;
} mtx_t;

void mutex_init(mtx_t *p_mutex);
void mutex_lock(mtx_t *p_mutex);
void mutex_unlock(mtx_t *p_mutex);
void mutex_deinit(mtx_t *p_mutex);

// 
// condition variable
// 
typedef struct cnd_s {
	CONDITION_VARIABLE cv;
} cnd_t;

void cnd_init(cnd_t *p_condvar);
void cnd_wake_one(cnd_t *p_condvar);
void cnd_wake_all(cnd_t *p_condvar);
void cnd_wait(cnd_t *p_condvar, mtx_t *p_mutex);


// 
// THREAD POOL
// 
enum TPTASK_PRIORITY {
	TPTP_LOW = 0,
	TPTP_NORMAL,
	TPTP_HIGH
};

typedef struct tpstatus_s {
	int active_threads;
	int completed_tasks;
	int remaining_tasks;
} tpstatus_t;

typedef (*TASKPROC)(tpstatus_t status, void *arg);

typedef struct tptask_s {
	int priority;
	void *p_arg;
	TASKPROC taskproc;
} tptask_t;

typedef struct threadpool_s {
	int tasks_capacity;
	int current_task;
	tptask_t *p_tasks;

	int num_of_threads;
	thread_t *p_threads;

	mtx_t mutex;
	cnd_t cvtask;
	cnd_t cvfinish;

	int state;
} threadpool_t;

int threadpool_init(threadpool_t *p_tp, int tasks_limit);
int threadpool_add_task(threadpool_t *p_tp, int priority, TASKPROC taskproc, void *p_arg);
int threadpool_suspend(threadpool_t *p_tp);
int threadpool_resume(threadpool_t *p_tp);
int threadpool_set_state(threadpool_t *p_tp, int _state);
int threadpool_join(threadpool_t *p_tp);
int threadpool_free(threadpool_t *p_tp);