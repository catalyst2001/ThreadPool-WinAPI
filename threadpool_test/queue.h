#pragma once
#include "threads.h"

#define assert(x) if(!(x)) __asm int 3h;

#define MAKE_PTR(ptr, elsz, pos) ((char *)ptr + (pos * elsz))
#define NOTHINGMACRO(x)

// ---------------------
// INLINE THREAD UNSAFE STACK
// ---------------------
#define STACK_DECLARE(var_name, _type) \
	struct { \
		int size; \
		int position; \
		_type *p_data; \
	} var_name;

#define STACK_INIT(var_name, _type, _size, _position) \
	 { \
		(var_name)->size = _size; \
		(var_name)->position = _position; \
		(var_name)->p_data = (_type *)malloc(sizeof(_type) * (var_name)->size); \
		assert((var_name)->p_data); \
	 }

#define STACK_PUSH(var_name, _element) \
	if((var_name)->position < (var_name)->size) { \
		(var_name)->p_data[(var_name)->position] = _element; \
		(var_name)->position++; \
	}

#define STACK_POP(var_name) \
	(var_name)->p_data[(var_name)->position]; \
	if((var_name)->position > 0) \
		(var_name)->position--;

#define STACK_FREE(var_name) free((var_name)->p_data)


// ---------------------
// INLINE THREAD UNSAFE HEAP ARRAY
// ---------------------
#define DECLARE_HEAP_ARRAY(var_name, type) \
	struct { \
		int capacity; \
		int size; \
		type *p_data; \
	} var_name;

#define HEAP_ARRAY_INIT(var_name, type, __capacity) \
	(var_name)->capacity = __capacity; \
	(var_name)->size = 0; \
	(var_name)->p_data = (type *)calloc(__capacity, sizeof(type)); \
	assert((var_name)->p_data);

#define HEAP_ARRAY_PUSH(var_name, type, val) \
	if((var_name)->size < (var_name)->capacity) \
		(var_name)->p_data[(var_name)->size] = val;

#define HEAP_ARRAY_POP(var_name, type) \
	(var_name)->p_data[(var_name)->size]; \
	if((var_name)->size > 0) \
			(var_name)->size--;

#define HEAP_ARRAY_GET(var_name, index) ((var_name)->p_data[index])

#define HEAP_ARRAY_COPY(var_name_to, var_name_from) \
	for(int i = 0; (i < (var_name_to)->capacity) && (i < (var_name_from)->capacity); i++) \
		(var_name_to)->p_data[i] = (var_name_from)->p_data[i];

#define HEAP_ARRAY_RESET(var_name) ((var_name)->size) = 0;

#define HEAP_ARRAY_REMOVE(var_name, index) \
		for(int i = index; i < (var_name)->capacity - 1; i++) \
			(var_name)->p_data[i] = (var_name)->p_data[i + 1];

#define HEAP_ARRAY_FREE(var_name) \
	free((var_name)->p_data);

// ---------------------
// THREAD UNSAFE STACK
// ---------------------

// 
// stack status
// 
enum STACK_STATUS {
	STACK_OK = 0,
	STACK_OUTOFMEMORY,
	STACK_OVERFLOW,
	STACK_UNDERFLOW
};

typedef struct stack_s {
	int size;
	int elem_size;
	int current_pos;
	char *p_data;
} stack_t;
NOTHINGMACRO(sizeof(stack_t))

int stack_init(stack_t *p_stack, int elem_size, int size);
int stack_push(stack_t *p_stack, void *p_data);
int stack_pop(void *p_dst, stack_t *p_stack);
int stack_pos(stack_t *p_stack, int pos);
void stack_free(stack_t *p_stack);

// ---------------------
// THREAD SAFE STACK
// ---------------------
typedef struct tsstack_s {
	int size;
	int elem_size;
	int current_pos;
	char *p_data;
	mtx_t mtx;
} tsstack_t;
NOTHINGMACRO(sizeof(tsstack_t))

int tsstack_init(tsstack_t *p_stack, int elem_size, int size);
int tsstack_push(tsstack_t *p_stack, void *p_data);
int tsstack_pop(void *p_dst, tsstack_t *p_stack);
int tsstack_pos(tsstack_t *p_stack, int pos);
void tsstack_free(tsstack_t *p_stack);

enum QUEUE_STATUS {
	QUEUE_OK = 0,
	QUEUE_OUTOFMEMORY,
	QUEUE_OVERFLOW,
	QUEUE_UNDERFLOW
};

// ---------------------
// THREAD UNSAFE QUEUE
// ---------------------
typedef struct queue_s {
	int elem_size;
	int capacity;
	int size;
	int front;
	int rear;
	char *p_data;
} queue_t;
NOTHINGMACRO(sizeof(queue_t))

#define queue_empty(p) ((p)->size)
int queue_init(queue_t *p_queue, int elem_size, int count);
int queue_push(queue_t *p_queue, void *p_data);
int queue_size(queue_t *p_queue);
int queue_pop_front(void *p_dst, queue_t *p_queue);
int queue_pop_rear(void *p_dst, queue_t *p_queue);
int queue_free(queue_t *p_queue);


// ---------------------
// THREAD SAFE QUEUE
// ---------------------
typedef struct tsqueue_s {
	int elem_size;
	int capacity;
	int size;
	int front;
	int rear;
	char *p_data;
	mtx_t mtx;
} tsqueue_t;
NOTHINGMACRO(sizeof(tsqueue_t))

int tsqueue_init(tsqueue_t *p_queue, int elem_size, int count);
int tsqueue_push(tsqueue_t *p_queue, void *p_data);
int tsqueue_size(tsqueue_t *p_queue);
int tsqueue_pop_front(void *p_dst, tsqueue_t *p_queue);
int tsqueue_pop_rear(void *p_dst, tsqueue_t *p_queue);
int tsqueue_free(tsqueue_t *p_queue);

