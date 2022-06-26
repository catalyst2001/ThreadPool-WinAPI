#pragma once
#include <stdio.h>

// ------------------------------------------------- 
// COMMON ENGINE API
// 
// образует единый шаблон работы с вводом/выводом для клиента/сервера
// ------------------------------------------------- 

int comm_init();
int comm_shutdown();

// ---------------------- 
// move this to API
// ---------------------- 
typedef struct commout_s {
	void (*comm_printf)(const char *p_format, ...);
	void (*comm_dprintf)(unsigned char *p_color, const char *p_format, ...);
	void (*comm_warning)(const char *p_format, ...);
	void (*comm_error)(const char *p_format, ...);
	void (*comm_log)(const char *p_format, ...);
	int  (*comm_fatal)(const char *p_format, ...);
} commout_t;