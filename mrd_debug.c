#include "mrd_debug.h"
#include "mrl_logger.h"
#include "mrs_misc.h"
#include "mrs_strings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef malloc
#undef calloc
#undef realloc
#undef free

#define MAX_ALLOCATIONS 4096

global_variable int current_allocation_id = 0;

global_variable struct Allocation allocations[MAX_ALLOCATIONS];
global_variable size_t allocations_count = 0;

void MRS_get_code_snippet(const char *file_name, int line, MRS_String *dest)
{
	FILE *file = fopen(file_name, "r");

	char read_buffer[256];

	int current_line = 1;
	while (fgets(read_buffer, sizeof(read_buffer), file)) {
		if (current_line == line) {
			break;
		}
		current_line++;
	}

	fclose(file);
	char *snippet_end = strchr(read_buffer, ';');

	MRS_setstrn(dest, read_buffer, snippet_end - read_buffer,
		    snippet_end - read_buffer);

	MRS_remove_whitespace(dest);
	return;
}

void *MRD_malloc(size_t size, const char *file, int line)
{
	char text[64];
	sprintf(text, "%d => ", current_allocation_id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu ", size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("malloc ", MRL_SEVERITY_ALT_INFO);
	MRL_log("allocated ", MRL_SEVERITY_DEFAULT);

	MRL_log("in ", MRL_SEVERITY_DEFAULT);

	sprintf(text, "%s:%d ", file, line);
	MRL_log(text, MRL_SEVERITY_OK);

	void *ptr = malloc(size);
	if (ptr != NULL) {
		allocations[allocations_count] =
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .deallocated = false };

		allocations_count++;
		current_allocation_id++;
	} else {
		MRL_log("FAILED TO ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	MRS_String code_snippet;
	MRS_init(256, "", 0, &code_snippet);
	MRS_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *MRD_calloc(size_t nmemb, size_t size, const char *file, int line)
{
	char text[64];

	sprintf(text, "%d => ", current_allocation_id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu ", size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("calloc ", MRL_SEVERITY_ALT_ALT_INFO);
	MRL_log("allocated ", MRL_SEVERITY_DEFAULT);

	MRL_log("in ", MRL_SEVERITY_DEFAULT);

	sprintf(text, "%s:%d ", file, line);
	MRL_log(text, MRL_SEVERITY_OK);

	void *ptr = calloc(nmemb, size);
	if (ptr != NULL) {
		allocations[allocations_count] =
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .deallocated = false };

		allocations_count++;
		current_allocation_id++;
	} else {
		MRL_log("FAILED TO ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	MRS_String code_snippet;
	MRS_init(256, "", 0, &code_snippet);
	MRS_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *MRD_realloc(void *ptr, size_t size, const char *file, int line)
{
	size_t allocation_idx = 420;
	for (size_t i = 0; i < allocations_count; i++) {
		if (ptr == allocations[i].ptr) {
			allocation_idx = i;
			allocations[i].deallocated = true;
			break;
		}
	}

	char text[64];

	sprintf(text, "%d>%d => ", allocations[allocation_idx].id,
		current_allocation_id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu>%lu ", allocations[allocation_idx].size, size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("realloc ", MRL_SEVERITY_INFO);
	MRL_log("allocated ", MRL_SEVERITY_DEFAULT);

	MRL_log("in ", MRL_SEVERITY_DEFAULT);

	sprintf(text, "%s:%d ", file, line);
	MRL_log(text, MRL_SEVERITY_OK);

	void *realloc_ptr = realloc(ptr, size);
	if (realloc_ptr != NULL) {
		allocations[allocations_count] =
			(struct Allocation){ .ptr = realloc_ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .deallocated = false };
		allocations_count++;
		current_allocation_id++;
	} else {
		MRL_log("FAILED TO REALLOCATE ", MRL_SEVERITY_ERROR);
	}

	MRS_String code_snippet;
	MRS_init(256, "", 0, &code_snippet);
	MRS_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	return realloc_ptr;
}

void MRD_free(void *ptr, const char *file, int line)
{
	struct Allocation allocation;
	for (size_t i = 0; i < allocations_count; i++) {
		if (ptr == allocations[i].ptr) {
			allocation = allocations[i];
			allocation.deallocated = true;
			break;
		}
	}

	char text[64];

	sprintf(text, "%d => ", allocation.id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu ", allocation.size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("free'd ", MRL_SEVERITY_WARNING);

	MRL_log("in ", MRL_SEVERITY_DEFAULT);

	sprintf(text, "%s:%d ", file, line);
	MRL_log(text, MRL_SEVERITY_OK);

	MRS_String code_snippet;
	MRS_init(256, "", 0, &code_snippet);
	MRS_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	free(ptr);
}
