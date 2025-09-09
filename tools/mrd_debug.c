#include "mrd_debug.h"
#include "../mrl_logger.h"

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

global_variable int current_allocation_id = 0;
global_variable struct Allocation active_allocations[MAX_ACTIVE_ALLOCATIONS];

global_variable void *(*real_malloc)(size_t) = NULL;
global_variable void *(*real_calloc)(size_t, size_t) = NULL;
global_variable void *(*real_realloc)(void *, size_t) = NULL;
global_variable void (*real_free)(void *) = NULL;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

internal void MRD_init_real_malloc(void)
{
#ifdef MRD_LOG_LEVEL_ALL
	MRL_logln("Initializing mrd_debug malloc", MRL_SEVERITY_INFO);
#endif

	real_malloc = dlsym(RTLD_NEXT, "malloc");
	if (real_malloc == NULL) {
		MRL_logln("FAILED TO FIND malloc", MRL_SEVERITY_ERROR);
	}
}

internal void MRD_init_real_calloc(void)
{
#ifdef MRD_LOG_LEVEL_ALL
	MRL_logln("Initializing mrd_debug calloc", MRL_SEVERITY_INFO);
#endif

	real_calloc = dlsym(RTLD_NEXT, "calloc");
	if (real_calloc == NULL) {
		MRL_logln("FAILED TO FIND calloc", MRL_SEVERITY_ERROR);
	}
}

internal void MRD_init_real_realloc(void)
{
#ifdef MRD_LOG_LEVEL_ALL
	MRL_logln("Initializing mrd_debug realloc", MRL_SEVERITY_INFO);
#endif

	real_realloc = dlsym(RTLD_NEXT, "realloc");
	if (real_realloc == NULL) {
		MRL_logln("FAILED TO FIND realloc", MRL_SEVERITY_ERROR);
	}
}

internal void MRD_init_real_free(void)
{
#ifdef MRD_LOG_LEVEL_ALL
	MRL_logln("Initializing mrd_debug free", MRL_SEVERITY_INFO);
#endif

	real_free = dlsym(RTLD_NEXT, "free");
	if (real_free == NULL) {
		MRL_logln("FAILED TO FIND free", MRL_SEVERITY_ERROR);
	}
}

#pragma GCC diagnostic pop

// returns 1 if true
internal int MRD_can_free_allocation(struct Allocation allocation)
{
	if (!allocation.active) {
		if (allocation.reallocated_to == NULL) {
			return 1;
		}
		if (!allocation.reallocated_to->active) {
			return 1;
		}
	}
	return 0;
}

// populates first available slot
internal void
MRD_add_allocation_to_active_allocations(struct Allocation new_allocation)
{
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (MRD_can_free_allocation(active_allocations[i])) {
			active_allocations[i] = new_allocation;
			current_allocation_id++;
			return;
		}
	}
	char err[64];
	sprintf(err, "MAX_ACTIVE_ALLOCATIONS reached (%d)",
		MAX_ACTIVE_ALLOCATIONS);
	MRL_logln(err, MRL_SEVERITY_ERROR);
}

internal void MRD_log_command(MRD_command command, size_t size,
			      struct Allocation *realloc_src)
{
	char text[MAX_LOG_LENGTH];

	if (command == MRD_COMMAND_REALLOC) {
		sprintf(text, "allocation (%d>%d) of ", realloc_src->id,
			current_allocation_id);
		MRL_log(text, MRL_SEVERITY_DEFAULT);

	} else {
		sprintf(text, "allocation (%d) of ", current_allocation_id);
	}
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	if (command == MRD_COMMAND_REALLOC) {
		sprintf(text, "%lu>%lu ", realloc_src->size, size);
	} else {
		sprintf(text, "%lu ", size);
	}
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);
	switch (command) {
	case MRD_COMMAND_MALLOC:
		MRL_log("malloc allocated ", MRL_SEVERITY_INFO);
		break;
	case MRD_COMMAND_CALLOC:
		MRL_log("calloc allocated ", MRL_SEVERITY_INFO);
		break;
	case MRD_COMMAND_REALLOC:
		MRL_log("realloc allocated ", MRL_SEVERITY_ALT_INFO);
		break;
	case MRD_COMMAND_FREE:
		MRL_log("free'd", MRL_SEVERITY_WARNING);
		break;
	default:
		break;
	}
	MRL_logln("", MRL_SEVERITY_DEFAULT);
}

void *malloc(size_t size)
{
	if (real_malloc == NULL) {
		MRD_init_real_malloc();
	}

#ifndef MRD_LOG_LEVEL_ERR_ONLY
	MRD_log_command(MRD_COMMAND_MALLOC, size, NULL);
#endif

	void *ptr = real_malloc(size);
	if (ptr != NULL) {
		memset(ptr, MRM_CAFE_BABE, size);
		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = TRUE,
					     .reallocated_to = NULL });

	} else {
#ifdef MRD_LOG_LEVEL_ERR_ONLY
		MRD_log_command(MRD_COMMAND_MALLOC, size, NULL);
#endif

		MRL_logln("FAILED TO MALLOC ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	return ptr;
}

void *calloc(size_t nmemb, size_t size)
{
	if (real_calloc == NULL) {
		MRD_init_real_calloc();
	}

#ifndef MRD_LOG_LEVEL_ERR_ONLY
	MRD_log_command(MRD_COMMAND_CALLOC, size, NULL);
#endif

	void *ptr = real_calloc(nmemb, size);
	if (ptr != NULL) {
		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = TRUE,
					     .reallocated_to = NULL });

	} else {
#ifdef MRD_LOG_LEVEL_ERR_ONLY
		MRD_log_command(MRD_COMMAND_CALLOC, size, NULL);
#endif

		MRL_logln("FAILED TO CALLOC ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	return ptr;
}

void *realloc(void *ptr, size_t size)
{
	if (real_realloc == NULL) {
		MRD_init_real_realloc();
	}

	struct Allocation *src_allocation = NULL;
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (ptr == active_allocations[i].ptr) {
			src_allocation = &active_allocations[i];
			break;
		}
	}

#ifndef MRD_LOG_LEVEL_ERR_ONLY
	MRD_log_command(MRD_COMMAND_REALLOC, size, src_allocation);
#endif

	void *realloc_ptr = real_realloc(ptr, size);
	if (realloc_ptr != NULL) {
		memset(realloc_ptr, MRM_CAFE_BABE, size);
		src_allocation->active = FALSE;
		src_allocation->reallocated_to = realloc_ptr;

		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = realloc_ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = TRUE,
					     .reallocated_to = NULL });

	} else {
#ifndef MRD_LOG_LEVEL_ERR_ONLY
		MRD_log_command(MRD_COMMAND_REALLOC, size, src_allocation);
#endif

		MRL_logln("FAILED TO REALLOCATE ", MRL_SEVERITY_ERROR);
	}

	return realloc_ptr;
}

void free(void *ptr)
{
	if (real_free == NULL) {
		MRD_init_real_free();
	}

	struct Allocation *allocation = NULL;
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (ptr == active_allocations[i].ptr) {
			allocation = &active_allocations[i];
			break;
		}
	}

	if (ptr == NULL) {
		MRL_logln("ATTEMPTED TO FREE NULL", MRL_SEVERITY_ERROR);
	}
#ifndef MRD_LOG_LEVEL_ERR_ONLY
	else {
		MRD_log_command(MRD_COMMAND_FREE, allocation->size, NULL);
	}
#endif

	real_free(ptr);
	allocation->active = FALSE;
}
