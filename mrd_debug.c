#include "mrd_debug.h"
#include "mrl_logger.h"
#include "mrs_strings.h"

#undef malloc
#undef calloc
#undef realloc
#undef free

#include <stdio.h>
#include <stdlib.h>

global_variable int current_allocation_id = 0;
global_variable struct Allocation active_allocations[MAX_ACTIVE_ALLOCATIONS];

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
			      struct Allocation *realloc_src,
			      const char *file_name, int line)
{
	char text[MAX_LOG_LENGTH];
	if (command == MRD_COMMAND_REALLOC) {
		sprintf(text, "allocation (%d>%d) of ", realloc_src->id,
			current_allocation_id);

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
		MRL_log("free'd ", MRL_SEVERITY_WARNING);
		break;
	default:
		break;
	}

	MRL_log("in ", MRL_SEVERITY_DEFAULT);
	sprintf(text, "%s:%d ", file_name, line);
	MRL_log(text, MRL_SEVERITY_OK);

	// cant call MRS_init due to it calling malloc
	MRS_String code_snippet;
	code_snippet.value = malloc(sizeof(char) * (MAX_SNIPPET_LEN + 1));
	code_snippet.value[MAX_SNIPPET_LEN] = '\0';
	code_snippet.capacity = MAX_SNIPPET_LEN;
	code_snippet.len = 0;

	MRD_get_code_snippet(file_name, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_logln("", MRL_SEVERITY_DEFAULT);
}

void *MRD_malloc(size_t size, const char *file_name, int line)
{
	MRD_log_command(MRD_COMMAND_MALLOC, size, NULL, file_name, line);

	void *ptr = malloc(size);
	if (ptr != NULL) {
		memset(ptr, MRM_CAFE_BABE, size);
		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = TRUE,
					     .reallocated_to = NULL });

	} else {
		MRD_log_command(MRD_COMMAND_MALLOC, size, NULL, file_name,
				line);
		MRL_logln("FAILED TO MALLOC ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	return ptr;
}

void *MRD_calloc(size_t nmemb, size_t size, const char *file_name, int line)
{
	MRD_log_command(MRD_COMMAND_CALLOC, size, NULL, file_name, line);

	void *ptr = calloc(nmemb, size);
	if (ptr != NULL) {
		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = TRUE,
					     .reallocated_to = NULL });

	} else {
		MRD_log_command(MRD_COMMAND_CALLOC, size, NULL, file_name,
				line);
		MRL_logln("FAILED TO CALLOC ALLOCATE ", MRL_SEVERITY_ERROR);
	}

	return ptr;
}

void *MRD_realloc(void *ptr, size_t size, const char *file_name, int line)
{
	struct Allocation *src_allocation = NULL;
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (ptr == active_allocations[i].ptr) {
			src_allocation = &active_allocations[i];
			break;
		}
	}

	MRD_log_command(MRD_COMMAND_REALLOC, size, src_allocation, file_name,
			line);

	void *realloc_ptr = realloc(ptr, size);
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
		MRD_log_command(MRD_COMMAND_REALLOC, size, src_allocation,
				file_name, line);

		MRL_logln("FAILED TO REALLOCATE ", MRL_SEVERITY_ERROR);
	}

	return realloc_ptr;
}

void MRD_free(void *ptr, const char *file_name, int line)
{
	struct Allocation *allocation = NULL;
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (ptr == active_allocations[i].ptr) {
			allocation = &active_allocations[i];
			break;
		}
	}

	if (ptr == NULL) {
		MRL_logln("ATTEMPTED TO FREE NULL", MRL_SEVERITY_ERROR);
	} else {
		(void)file_name;
		(void)line;
		MRD_log_command(MRD_COMMAND_FREE, allocation->size, NULL,
				file_name, line);
	}

	free(ptr);
	allocation->active = FALSE;
}
