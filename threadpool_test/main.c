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

int main()
{
	threadpool_t tp;
	threadpool_init(&tp, 1024);
	while (1) {
		/*getchar();*/
		printf("Added 20 tasks!\n");
		for (int i = 0; i < 20; i++)
			threadpool_add_task(&tp, TPTP_HIGH, task_proc, NULL);

		threadpool_wait_tasks_execution(&tp);
	}
	threadpool_join(&tp);
	threadpool_free(&tp);
	return 0;
}