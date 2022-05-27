#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
// ------------------------------------
// MEMORY MANAGER
// 
// ------------------------------------

#define MM_NOTHINGMACRO(x)

// 
// MEMORY ALLOCATION
// 
#define MM_NOOPTION 0  //not cleaned memory
#define MM_CLEAR 1	   //cleaner memory (filled by NULL)

void *mm_alloc(const char *p_description, int option, size_t size);
void *mm_free(void *p_block);
int   mm_get_error();

// 
// DEBUG INFORMATION
// 
typedef struct mm_block_information_s {
	const char *description;
	size_t block_size;
} mm_block_information_t;

// 
// ALLOCATED MEMORY BLOCKS ENUMERATION
// EXAMPLE:
// 
// mm_block_information_t blockinf;
// if(mm_first_block(&blockinf)) {
//		do {
//			/* use information in blockinf */
//			// ...
//			
//		} while(mm_next_block(&blockinf));
// }
// 
int mm_first_block(mm_block_information_t *p_dst_inf);
bool mm_next_block(mm_block_information_t *p_dst_inf);

#if !defined(__PRETTY_FUNCTION__) && defined(_MSC_VER)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define MEM_ALLOC(type, size) ((type *)mm_alloc(__PRETTY_FUNCTION__, sizeof(type) * size))

// ------------------------------------
// HANDLE API
// ------------------------------------
typedef void *handle_t;

#define HANDLE_MAGIC 0x4C444E48

typedef enum HANDLE_STATUS_ {
	HS_OK = 0,
	HS_OUT_OF_MEMORY,
	HS_INVALID_SIZE,
	HS_INVALID_HANDLE
} HANDLE_STATUS;

HANDLE_STATUS handle_create(handle_t *p_dst_handle, const char *p_description, void *p_data, size_t size);
void         *handle_get_object(handle_t handle);
size_t        handle_get_object_size(handle_t handle);
bool          handle_is_valid(handle_t handle);
HANDLE_STATUS handle_duplicate(handle_t *p_to_handle, handle_t from);
HANDLE_STATUS handle_close(handle_t handle);

// 
// HANDLE API DISPATCH TABLE
// TODO: MOVE THIS TO SDK!
// 
typedef struct handleapidt_s {
	HANDLE_STATUS (*handle_create)(handle_t *p_dst_handle, const char *p_description, size_t size);
	void         *(*handle_get_object)(handle_t handle);
	size_t        (*handle_get_object_size)(handle_t handle);
	bool          (*handle_is_valid)(handle_t handle);
	handle_t      (*handle_duplicate)(handle_t from);
	HANDLE_STATUS (*handle_close)(handle_t handle);
} handleapidt_t;