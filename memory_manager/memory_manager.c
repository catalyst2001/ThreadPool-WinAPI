#include "memory_manager.h"
#include <malloc.h>

typedef struct mm_block_s {
	size_t block_size;
	struct mm_block_s *p_last;
	struct mm_block_s *p_next;
	const char *p_description;
} mm_block_t;
MM_NOTHINGMACRO(sizeof(mm_block_t));

mm_block_t *p_begin_block, p_end_block;

void *mm_alloc(const char *p_description, int option, size_t size)
{
	
	return NULL;
}

void *mm_free(void *p_block)
{
	return NULL;
}

int mm_get_error()
{
	return 0;
}

void *mm_begin_block()
{
	return NULL;
}

void *mm_end_block()
{
	return NULL;
}

void *mm_prev_block(void *p_block)
{
	return NULL;
}

void *mm_next_block(void *p_block)
{
	return NULL;
}

unsigned long mm_alloc_timestamp(void *p_block)
{
	return 0;
}

const char *mm_block_description(void *p_block)
{
	return NULL;
}

// ----------------------------------------------------- 
// HANDLE API
// ----------------------------------------------------- 
typedef struct handle_body_object_s {
	int magic;
	size_t handle_object_size;
	const char *p_description;
	struct handle_body_object_s *p_last;
	struct handle_body_object_s *p_next;
} handle_body_object_t;
MM_NOTHINGMACRO(sizeof(handle_body_object_t));

HANDLE_STATUS handle_create(handle_t *p_dst_handle, const char *p_description, size_t size)
{
	handle_body_object_t *p_handle_body_object = mm_alloc("handle_create", MM_CLEAR, sizeof(handle_body_object_t) + size);
	assert(p_handle_body_object);
	p_handle_body_object->handle_object_size = size;

	return HS_OK;
}

void *handle_get_object(handle_t handle)
{
	return NULL;
}

size_t handle_get_object_size(handle_t handle)
{
	return size_t();
}

bool handle_is_valid(handle_t handle)
{
	return false;
}

handle_t handle_duplicate(handle_t from)
{
	return handle_t();
}

HANDLE_STATUS handle_close(handle_t handle)
{
	return HANDLE_STATUS();
}