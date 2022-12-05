#include <stdio.h>
#include <Windows.h>
#include "threads.h"

void task_proc(const tpstatus_t *p_statistics, void *param)
{
	int c = 0;
	for (int i = 0; i < 10000; i++)
		for (int k = 0; k < 10000; k++)
			c = !c;

	printf("result %d  Task %d finished!\n", c, p_statistics->task_sequence);
}

void unlimited_task_proc(const tpstatus_t *p_statistics, void *param)
{
	while (1);
}

int main()
{
	threadpool_t tp;
	threadpool_init(&tp, 1024);

	int workers_ids[11];
	while (1) {
		/*getchar();*/
		printf("Added 20 tasks!\n");
		for (int i = 0; i < 10; i++)
			threadpool_add_task(&tp, &workers_ids[i], TPTP_HIGH, task_proc, NULL);

		// add hang task
		threadpool_add_task(&tp, &workers_ids[10], TPTP_HIGH, unlimited_task_proc, NULL);

		for (int i = 0; i < 11; i++)
			printf("Task %d status %s\n", i, (workers_ids[i] != TASK_NO_EXECUTED) ? "OK" : "EXECUTING");

		threadpool_wait_tasks_execution(&tp);
	}
	threadpool_join(&tp);
	threadpool_free(&tp);
	return 0;
}