#include "mrd_debug.h"
#include "mrl_logger.h"
#include "mrm_misc.h"
#include "mrs_strings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef malloc
#undef calloc
#undef realloc
#undef free

global_variable int current_allocation_id = 0;

global_variable struct Allocation active_allocations[MAX_ACTIVE_ALLOCATIONS];
global_variable size_t active_allocations_length = 0;

// returns 1 if true
int MRD_can_free_allocation(struct Allocation allocation)
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

// searches for the first empty slot
void MRD_add_allocation_to_active_allocations(struct Allocation new_allocation)
{
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (MRD_can_free_allocation(active_allocations[i])) {
			active_allocations[i] = new_allocation;
			return;
		}
	}
}

void MRD_get_code_snippet(const char *file_name, int line, MRS_String *dest)
{
	FILE *file = fopen(file_name, "r");

	// cant call MRS_init due to it calling malloc
	MRS_String read_buffer;
	read_buffer.value = malloc(sizeof(char) * (MAX_SNIPPET_LEN + 1));
	read_buffer.value[MAX_SNIPPET_LEN] = '\0';
	read_buffer.capacity = MAX_SNIPPET_LEN;
	read_buffer.len = 0;

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

	MRS_filter(&read_buffer, '\n');
	MRS_filter(&read_buffer, '\t');

	MRS_setstrn(dest, read_buffer.value, read_buffer.len, read_buffer.len);
	free(read_buffer.value);
	return;
}

void *MRD_malloc(size_t size, const char *file, int line)
{
	char text[MAX_SNIPPET_LEN];
	sprintf(text, "allocation (%d) of ", current_allocation_id);
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
		memset(ptr, MRM_CAFE_BABE, size);
		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = true,
					     .reallocated_to = NULL });

		active_allocations_length++;
		current_allocation_id++;
	} else {
		MRL_log("FAILED TO ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	// cant call MRS_init due to it calling malloc
	MRS_String code_snippet;
	code_snippet.value = malloc(sizeof(char) * (MAX_SNIPPET_LEN + 1));
	code_snippet.value[MAX_SNIPPET_LEN] = '\0';
	code_snippet.capacity = MAX_SNIPPET_LEN;
	code_snippet.len = 0;

	MRD_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *MRD_calloc(size_t nmemb, size_t size, const char *file, int line)
{
	char text[MAX_SNIPPET_LEN];

	sprintf(text, "allocation (%d) of ", current_allocation_id);
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
		active_allocations[active_allocations_length] =
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = true,
					     .reallocated_to = NULL };

		active_allocations_length++;
		current_allocation_id++;
	} else {
		MRL_log("FAILED TO ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	// cant call MRS_init due to it calling malloc
	MRS_String code_snippet;
	code_snippet.value = malloc(sizeof(char) * (MAX_SNIPPET_LEN + 1));
	code_snippet.value[MAX_SNIPPET_LEN] = '\0';
	code_snippet.capacity = MAX_SNIPPET_LEN;
	code_snippet.len = 0;

	MRD_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *MRD_realloc(void *ptr, size_t size, const char *file, int line)
{
	struct Allocation *src_allocation = NULL;
	for (size_t i = 0; i < active_allocations_length; i++) {
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

	MRL_log("in ", MRL_SEVERITY_DEFAULT);

	sprintf(text, "%s:%d ", file, line);
	MRL_log(text, MRL_SEVERITY_OK);

	void *realloc_ptr = realloc(ptr, size);
	if (realloc_ptr != NULL) {
		memset(realloc_ptr, MRM_CAFE_BABE, size);
		src_allocation->active = false;
		src_allocation->reallocated_to = src_allocation;

		active_allocations[active_allocations_length] =
			(struct Allocation){ .ptr = realloc_ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = true,
					     .reallocated_to = NULL };

		active_allocations_length++;
		current_allocation_id++;
	} else {
		MRL_log("FAILED TO REALLOCATE ", MRL_SEVERITY_ERROR);
	}

	// cant call MRS_init due to it calling malloc
	MRS_String code_snippet;
	code_snippet.value = malloc(sizeof(char) * (MAX_SNIPPET_LEN + 1));
	code_snippet.value[MAX_SNIPPET_LEN] = '\0';
	code_snippet.capacity = MAX_SNIPPET_LEN;
	code_snippet.len = 0;

	MRD_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	return realloc_ptr;
}

void MRD_free(void *ptr, const char *file, int line)
{
	struct Allocation allocation;
	for (size_t i = 0; i < active_allocations_length; i++) {
		if (ptr == active_allocations[i].ptr) {
			allocation = active_allocations[i];
			break;
		}
	}

	char text[MAX_SNIPPET_LEN];

	sprintf(text, "allocation (%d) of ", allocation.id);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	sprintf(text, "%lu ", allocation.size);
	MRL_log(text, MRL_SEVERITY_OK);

	MRL_log("bytes ", MRL_SEVERITY_DEFAULT);

	MRL_log("free'd ", MRL_SEVERITY_WARNING);

	MRL_log("in ", MRL_SEVERITY_DEFAULT);

	sprintf(text, "%s:%d ", file, line);
	MRL_log(text, MRL_SEVERITY_OK);

	// cant call MRS_init due to it calling malloc
	MRS_String code_snippet;
	code_snippet.value = malloc(sizeof(char) * (MAX_SNIPPET_LEN + 1));
	code_snippet.value[MAX_SNIPPET_LEN] = '\0';
	code_snippet.capacity = MAX_SNIPPET_LEN;
	code_snippet.len = 0;

	MRD_get_code_snippet(file, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);

	free(code_snippet.value);
	allocation.active = false;

	MRL_log("\n\n", MRL_SEVERITY_DEFAULT);

	free(ptr);
}
