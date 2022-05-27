#include "memory_manager.h"
#include <Windows.h>
#include <malloc.h>

#ifndef UNSAFE_HEAP
typedef struct mm_block_s {
	size_t block_size;
	struct mm_block_s *p_last;
	struct mm_block_s *p_next;
	const char *p_description;
} mm_block_t;
MM_NOTHINGMACRO(sizeof(mm_block_t));

mm_block_t *p_begin_block = NULL;
mm_block_t *p_end_block = NULL;
#endif

void *mm_alloc(const char *p_description, int option, size_t size)
{
#ifndef UNSAFE_HEAP
	mm_block_t *p_block;
	if (option == MM_CLEAR) {
		p_block = (mm_block_t *)calloc(sizeof(mm_block_t) + size, 1);
	}
	else {
		p_block = (mm_block_t *)malloc(sizeof(mm_block_t) + size);
	}
	p_block->block_size = size;
#else
	//TODO: modify memory manager allocator
	if (option == MM_CLEAR)
		return (void *)calloc(size, 1);

	return (void *)malloc(size);
#endif
}

void *mm_free(void *p_block)
{
	return NULL;
}

int mm_get_error()
{
	return 0;
}

bool mm_next_block(void *p_block)
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

handle_body_object_t *p_begin_handle_object = NULL, *p_end_handle_object = NULL;

HANDLE_STATUS handle_create(handle_t *p_dst_handle, const char *p_description, void *p_data, size_t size)
{
	// check handle object size
	if (size <= 0)
		return HS_INVALID_SIZE;

	handle_body_object_t *p_handle_body_object = mm_alloc("hndlmgr -> handle_create()", MM_CLEAR, sizeof(handle_body_object_t) + size);
	if (p_handle_body_object) {
		p_handle_body_object->magic = HANDLE_MAGIC;
		p_handle_body_object->handle_object_size = size;
		p_handle_body_object->p_description = p_description;

		// previous handle exists
		if (p_end_handle_object) {
			p_end_handle_object->p_next = p_handle_body_object;
			p_handle_body_object->p_last = p_end_handle_object;
		}
		p_end_handle_object = p_handle_body_object;

		// end handle object not set
		if (!p_begin_handle_object)
			p_begin_handle_object = p_handle_body_object;

		*p_dst_handle = p_handle_body_object;
		return HS_OK;
	}
	assert(p_handle_body_object);
	return HS_OUT_OF_MEMORY;
}

void *handle_get_object(handle_t handle)
{
	return (char *)handle + sizeof(handle_body_object_t);
}

size_t handle_get_object_size(handle_t handle)
{
	return ((handle_body_object_t *)handle)->handle_object_size;
}

bool handle_is_valid(handle_t handle)
{
	__try {
		return ((handle_body_object_t *)handle)->magic == HANDLE_MAGIC;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
	return false;
}

HANDLE_STATUS handle_duplicate(handle_t *p_to_handle, handle_t from)
{
	if (handle_is_valid(from)) {
		handle_body_object_t *p_handle_object = (handle_body_object_t *)from;
		return handle_create(p_to_handle, p_handle_object->p_description, NULL, p_handle_object->handle_object_size);
	}
	return HS_INVALID_HANDLE;
}

HANDLE_STATUS handle_close(handle_t handle)
{
	if (!handle_is_valid(handle))
		return HS_INVALID_HANDLE;

	// delete link between two handles
	handle_body_object_t *p_handle_object = (handle_body_object_t *)handle;
	if (p_handle_object->p_last) {
		p_handle_object->p_last->p_next = p_handle_object->p_next;
		p_handle_object->p_next->p_last = p_handle_object->p_last;
	}
	mm_free(handle);
	return HS_OK;
}