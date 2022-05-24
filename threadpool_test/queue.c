#include "queue.h"

int stack_init(stack_t *p_stack, int elem_size, int size)
{
	p_stack->current_pos = 0;
	p_stack->elem_size = elem_size;
	p_stack->size = size;
	p_stack->p_data = (char *)calloc(p_stack->size, p_stack->elem_size);
	if (!p_stack->p_data) {
		assert(p_stack->p_data);
		return STACK_OUTOFMEMORY;
	}
	return STACK_OK;
}

int stack_push(stack_t *p_stack, void *p_data)
{
	if (p_stack->current_pos >= p_stack->size)
		return STACK_OVERFLOW;

	memcpy(MAKE_PTR(p_stack->p_data, p_stack->elem_size, p_stack->current_pos), p_data, p_stack->elem_size);
	p_stack->current_pos++;
	return STACK_OK;
}

int stack_pop(void *p_dst, stack_t *p_stack)
{
	if (p_stack->current_pos <= 0)
		return STACK_UNDERFLOW;

	memcpy(p_dst, MAKE_PTR(p_stack->p_data, p_stack->elem_size, p_stack->current_pos), p_stack->elem_size);
	return STACK_OK;
}

int stack_pos(stack_t *p_stack, int pos)
{
	int oldpos = p_stack->current_pos;
	p_stack->current_pos = pos;
	return oldpos;
}

void stack_free(stack_t *p_stack)
{
	if (p_stack->p_data)
		free(p_stack->p_data);
}

// THREAD SAFE STACK

int tsstack_init(tsstack_t *p_stack, int elem_size, int size)
{
	p_stack->current_pos = 0;
	p_stack->elem_size = elem_size;
	p_stack->size = size;
	p_stack->p_data = (char *)calloc(p_stack->size, p_stack->elem_size);
	if (!p_stack->p_data) {
		assert(p_stack->p_data);
		return STACK_OUTOFMEMORY;
	}
	mutex_init(&p_stack->mtx);
	return STACK_OK;
}

int tsstack_push(tsstack_t *p_stack, void *p_data)
{
	mutex_lock(&p_stack->mtx);
	if (p_stack->current_pos >= p_stack->size)
		return STACK_OVERFLOW;

	memcpy(MAKE_PTR(p_stack->p_data, p_stack->elem_size, p_stack->current_pos), p_data, p_stack->elem_size);
	p_stack->current_pos++;
	mutex_unlock(&p_stack->mtx);
	return STACK_OK;
}

int tsstack_pop(void *p_dst, tsstack_t *p_stack)
{
	mutex_lock(&p_stack->mtx);
	if (p_stack->current_pos <= 0)
		return STACK_UNDERFLOW;

	memcpy(p_dst, MAKE_PTR(p_stack->p_data, p_stack->elem_size, p_stack->current_pos), p_stack->elem_size);
	mutex_unlock(&p_stack->mtx);
	return STACK_OK;
}

int tsstack_pos(tsstack_t *p_stack, int pos)
{
	int oldpos;
	InterlockedExchange(&oldpos, p_stack->current_pos);
	InterlockedExchange(&p_stack->current_pos, pos);
	return oldpos;
}

void tsstack_free(tsstack_t *p_stack)
{
	if (p_stack->p_data)
		free(p_stack->p_data);

	mutex_deinit(&p_stack->mtx);
}

//////////////////////////////////////////////////////////////////////////////////////

int queue_init(queue_t *p_queue, int elem_size, int count)
{
	p_queue->capacity = count;
	p_queue->elem_size = elem_size;
	p_queue->front = 0;
	p_queue->rear = 0;
	p_queue->size = 0;
	p_queue->p_data = (char *)calloc(p_queue->capacity, p_queue->elem_size);
	if (!p_queue->p_data) {
		assert(p_queue->p_data);
		return QUEUE_OUTOFMEMORY;
	}
	return QUEUE_OK;
}

int queue_push(queue_t *p_queue, void *p_data)
{
	if (p_queue->size >= p_queue->capacity)
		return QUEUE_OVERFLOW;

	memcpy(MAKE_PTR(p_queue->p_data, p_queue->elem_size, p_queue->size), p_data, p_queue->elem_size);
	p_queue->size++;
	return QUEUE_OK;
}

int queue_size(queue_t *p_queue)
{
	return p_queue->size;
}

int queue_pop_front(void *p_dst, queue_t *p_queue)
{
	if (!p_queue->size)
		return QUEUE_UNDERFLOW;

	memcpy(p_dst, MAKE_PTR(p_queue->p_data, p_queue->elem_size, p_queue->size), p_queue->elem_size);
	p_queue->size--;
	return QUEUE_OK;
}

int queue_pop_rear(void *p_dst, queue_t *p_queue)
{

	p_queue->size--;
	return QUEUE_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
int tsqueue_init(tsqueue_t *p_queue, int elem_size, int count)
{
	return 0;
}
