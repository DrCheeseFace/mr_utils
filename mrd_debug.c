#include "mrd_debug.h"
#include "mrl_logger.h"
#include "mrm_misc.h"
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

global_variable int current_allocation_id = 0;
global_variable struct Allocation active_allocations[MAX_ACTIVE_ALLOCATIONS];

global_variable void *(*real_malloc)(size_t) = NULL;
global_variable void *(*real_calloc)(size_t, size_t) = NULL;
global_variable void *(*real_realloc)(void *, size_t) = NULL;
global_variable void (*real_free)(void *) = NULL;

internal void MRD_init_real_malloc(void)
{
	real_malloc = dlsym(RTLD_NEXT, "malloc");
	if (real_malloc == NULL) {
		fprintf(stderr, "LD_PRELOAD ERR: FAILED TO FIND malloc");
	}
}

internal void MRD_init_real_calloc(void)
{
	real_calloc = dlsym(RTLD_NEXT, "calloc");
	if (real_calloc == NULL) {
		fprintf(stderr, "LD_PRELOAD ERR: FAILED TO FIND calloc");
	}
}

internal void MRD_init_real_realloc(void)
{
	real_realloc = dlsym(RTLD_NEXT, "realloc");
	if (real_realloc == NULL) {
		fprintf(stderr, "LD_PRELOAD ERR: FAILED TO FIND realloc");
	}
}

internal void MRD_init_real_free(void)
{
	real_free = dlsym(RTLD_NEXT, "free");
	if (real_free == NULL) {
		fprintf(stderr, "LD_PRELOAD ERR: FAILED TO FIND free");
	}
}

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

void *malloc(size_t size)
{
	if (real_malloc == NULL) {
		MRD_init_real_malloc();
	}

	char text[MAX_LOG_LENGTH];
	sprintf(text, "allocation (%d) of ", current_allocation_id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu ", size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("malloc allocated ", MRL_SEVERITY_INFO);

	void *ptr = real_malloc(size);
	if (ptr != NULL) {
		memset(ptr, MRM_CAFE_BABE, size);
		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = true,
					     .reallocated_to = NULL });

	} else {
		MRL_log("FAILED TO ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *calloc(size_t nmemb, size_t size)
{
	if (real_calloc == NULL) {
		MRD_init_real_calloc();
	}

	char text[MAX_SNIPPET_LEN];

	sprintf(text, "allocation (%d) of ", current_allocation_id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu ", size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("calloc allocated ", MRL_SEVERITY_INFO);

	void *ptr = real_calloc(nmemb, size);
	if (ptr != NULL) {
		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = true,
					     .reallocated_to = NULL });

	} else {
		MRL_log("FAILED TO ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

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

	char text[MAX_SNIPPET_LEN];

	sprintf(text, "allocation (%d>%d) of ", src_allocation->id,
		current_allocation_id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu>%lu ", src_allocation->size, size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("realloc allocated ", MRL_SEVERITY_ALT_INFO);

	void *realloc_ptr = real_realloc(ptr, size);
	if (realloc_ptr != NULL) {
		memset(realloc_ptr, MRM_CAFE_BABE, size);
		src_allocation->active = false;
		src_allocation->reallocated_to = realloc_ptr;

		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = realloc_ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = true,
					     .reallocated_to = NULL });

	} else {
		MRL_log("FAILED TO REALLOCATE ", MRL_SEVERITY_ERROR);
	}

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

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

	char text[MAX_SNIPPET_LEN];

	sprintf(text, "allocation (%d) of ", allocation->id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu ", allocation->size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("free'd ", MRL_SEVERITY_WARNING);

	allocation->active = false;

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	real_free(ptr);
}
