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

global_variable int current_allocation_id = 0;

global_variable struct Allocation allocations[MAX_ALLOCATIONS];
global_variable size_t allocations_count = 0;

void MRS_get_code_snippet(const char *file_name, int line, MRS_String *dest)
{
	FILE *file = fopen(file_name, "r");

	MRS_String read_buffer;
	MRS_init(MAX_SNIPPET_LEN, "", 0, &read_buffer);

	int current_line = 1;
	while (fgets(read_buffer.value, read_buffer.capacity, file)) {
		read_buffer.len = strlen(read_buffer.value);

		if (current_line == line) {
			break;
		}

		if (MRS_strchr(&read_buffer, '\n') != NULL) {
			current_line++;
		}
	}
	fclose(file);

	MRS_remove_whitespace(&read_buffer);

	MRS_setstrn(dest, read_buffer.value, read_buffer.len, read_buffer.len);
	free(read_buffer.value);
	return;
}

void *MRD_malloc(size_t size, const char *file, int line)
{
	char text[MAX_SNIPPET_LEN];
	sprintf(text, "%d => ", current_allocation_id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu ", size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("malloc allocated ", MRL_SEVERITY_INFO);

	MRL_log("in ", MRL_SEVERITY_DEFAULT);

	sprintf(text, "%s:%d ", file, line);
	MRL_log(text, MRL_SEVERITY_OK);

	void *ptr = malloc(size);
	if (ptr != NULL) {
		allocations[allocations_count] =
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .freed = false,
					     .reallocated = false,
					     .reallocated_to_id = CAFE_BABE };

		allocations_count++;
		current_allocation_id++;
	} else {
		MRL_log("FAILED TO ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	MRS_String code_snippet;
	MRS_init(MAX_SNIPPET_LEN, "", 0, &code_snippet);
	MRS_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *MRD_calloc(size_t nmemb, size_t size, const char *file, int line)
{
	char text[MAX_SNIPPET_LEN];

	sprintf(text, "%d => ", current_allocation_id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu ", size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("calloc allocated ", MRL_SEVERITY_INFO);

	MRL_log("in ", MRL_SEVERITY_DEFAULT);

	sprintf(text, "%s:%d ", file, line);
	MRL_log(text, MRL_SEVERITY_OK);

	void *ptr = calloc(nmemb, size);
	if (ptr != NULL) {
		allocations[allocations_count] =
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .freed = false,
					     .reallocated = false,
					     .reallocated_to_id = CAFE_BABE };

		allocations_count++;
		current_allocation_id++;
	} else {
		MRL_log("FAILED TO ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	MRS_String code_snippet;
	MRS_init(MAX_SNIPPET_LEN, "", 0, &code_snippet);
	MRS_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *MRD_realloc(void *ptr, size_t size, const char *file, int line)
{
	size_t src_allocation_idx = CAFE_BABE;
	for (size_t i = 0; i < allocations_count; i++) {
		if (ptr == allocations[i].ptr) {
			src_allocation_idx = i;
			break;
		}
	}

	char text[MAX_SNIPPET_LEN];

	sprintf(text, "%d>%d => ", allocations[src_allocation_idx].id,
		current_allocation_id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu>%lu ", allocations[src_allocation_idx].size, size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("realloc allocated ", MRL_SEVERITY_ALT_INFO);

	MRL_log("in ", MRL_SEVERITY_DEFAULT);

	sprintf(text, "%s:%d ", file, line);
	MRL_log(text, MRL_SEVERITY_OK);

	void *realloc_ptr = realloc(ptr, size);
	if (realloc_ptr != NULL) {
		allocations[src_allocation_idx].freed = true;
		allocations[src_allocation_idx].reallocated = true;
		allocations[src_allocation_idx].reallocated_to_id =
			current_allocation_id;

		allocations[allocations_count] =
			(struct Allocation){ .ptr = realloc_ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .freed = false,
					     .reallocated = false,
					     .reallocated_to_id = CAFE_BABE };

		allocations_count++;
		current_allocation_id++;
	} else {
		MRL_log("FAILED TO REALLOCATE ", MRL_SEVERITY_ERROR);
	}

	MRS_String code_snippet;
	MRS_init(MAX_SNIPPET_LEN, "", 0, &code_snippet);
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
			break;
		}
	}

	char text[MAX_SNIPPET_LEN];

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
	MRS_init(MAX_SNIPPET_LEN, "", 0, &code_snippet);
	MRS_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	free(code_snippet.value);
	allocation.freed = true;

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	free(ptr);
}
