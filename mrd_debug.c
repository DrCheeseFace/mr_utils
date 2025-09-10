#include "mrd_debug.h"
#include "execinfo.h"
#include "mrl_logger.h"
#include "mrs_strings.h"
#include <string.h>
#include <unistd.h>

#undef malloc
#undef calloc
#undef realloc
#undef free

#include <stdio.h>
#include <stdlib.h>

global_variable int current_allocation_id = 0;
global_variable struct Allocation active_allocations[MAX_ACTIVE_ALLOCATIONS];

// cant call MRS_init due to it calling malloc
internal void MRS_init_code_snippet(MRS_String *dest)
{
	dest->value = malloc(sizeof(char) * (MAX_SNIPPET_LEN + 1));
	dest->value[MAX_SNIPPET_LEN] = '\0';
	dest->capacity = MAX_SNIPPET_LEN;
	dest->len = 0;
}

internal void MRD_log_err(const char *msg)
{
	MRL_log(DEBUG_LOG_HEAD, MRL_SEVERITY_INFO);
	MRL_logln(msg, MRL_SEVERITY_ERROR);
}

internal void MRD_get_code_snippet(const char *file_name, int line,
				   MRS_String *dest)
{
	FILE *file = fopen(file_name, "r");

	MRS_String read_buffer;
	MRS_init_code_snippet(&read_buffer);

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

internal long MRD_get_base_address(const char *path)
{
	int pid = getpid();
	char base_addr[BASE_ADDRESS_SIZE + 1];
	char command[256];
	sprintf(command,
		"cat /proc/%d/maps | grep %s | head -n 1 | awk '{print $1}' | cut -d'-' -f1",
		pid, path); // lol

	FILE *fp = popen(command, "r");
	char output_buffer[1024] = "";
	char output_full_out[1024] = "";
	while (fgets(output_buffer, sizeof(output_buffer), fp) != NULL) {
		strcat(output_full_out, output_buffer);
	}

	for (size_t i = 0; i < BASE_ADDRESS_SIZE; i++) {
		base_addr[i] = output_full_out[i];
	}

	long base_addr_long = strtol(base_addr, NULL, 16);

	return base_addr_long;
}

internal void unused MRD_log_backtrace(void)
{
	void *buffer[MAX_BACKTRACE_LENGTH];
	int nptrs = backtrace(buffer, MAX_BACKTRACE_LENGTH);

	char **symbols = backtrace_symbols(buffer, nptrs);
	if (symbols == NULL) {
		MRD_log_err("FAILED TO GET BACKTRACE SYMBOLS");
	}

	// get path of exec
	char *end_of_path = strchr(symbols[1], '(');
	char path[128];
	strncpy(path, symbols[1], end_of_path - symbols[1]);
	path[end_of_path - symbols[1]] = '\0';

	long base_addr_long = MRD_get_base_address(path);

	uint max_backtrace_depth_printout = MAX_BACKTRACE_DEPTH_PRINTOUT;
	if (nptrs < (int)max_backtrace_depth_printout) {
		max_backtrace_depth_printout = nptrs;
	}

	// 2 is used here to remove depth created in this file
	for (size_t i = 2; i < max_backtrace_depth_printout; i++) {
		// calc diff between base and call addr
		char call_addr[BASE_ADDRESS_SIZE + 1];
		call_addr[BASE_ADDRESS_SIZE] = '\0';
		char *open_bracket = strchr(symbols[i], '[');
		open_bracket += 3;
		strncpy(call_addr, open_bracket, BASE_ADDRESS_SIZE);

		long call_addr_long = strtol(call_addr, NULL, 16);
		long addr_diff = call_addr_long - base_addr_long;
		char addr_diff_hex[32];
		sprintf(addr_diff_hex, " +0x%lX", addr_diff);

		// build addr2line command
		char addr2line_command[128] = "";
		strcat(addr2line_command, "addr2line -e ");
		strcat(addr2line_command, path);
		strcat(addr2line_command, " ");
		strcat(addr2line_command, addr_diff_hex);

		// exec addr2line command
		FILE *fp = popen(addr2line_command, "r");
		char addr2line_output_buffer[256] = "";
		char addr2line_output_full_out[256] = "";
		while (fgets(addr2line_output_buffer,
			     sizeof(addr2line_output_buffer), fp) != NULL) {
			strcat(addr2line_output_full_out,
			       addr2line_output_buffer);
		}

		char *semi_colon = strchr(addr2line_output_full_out, ':');
		char line_string[6];
		strtok(semi_colon, " ");
		strcpy(line_string, semi_colon + 1);
		strtok(line_string, "\n");
		*semi_colon = '\0';

		char *start_func = strchr(symbols[i], '(') + 1;
		char *end_func = strchr(symbols[i], '+');
		char func_name[128] = "";
		strncpy(func_name, start_func, end_func - start_func);

		char log[512];
		sprintf(log, "%s:%s => %s()", addr2line_output_full_out,
			line_string, func_name);

		for (size_t indents = 0; indents < i - 1; indents++) {
			MRL_log("  ", MRL_SEVERITY_DEFAULT);
		}
		MRL_log("↪ ", MRL_SEVERITY_DEFAULT);
		MRL_logln(log, MRL_SEVERITY_DEFAULT);
	}

	free(symbols);
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

	MRD_log_err(err);
}

internal void MRD_log_command(MRD_command command, size_t size,
			      struct Allocation *realloc_src,
			      const char *file_name, int line)
{
	MRL_log(DEBUG_LOG_HEAD, MRL_SEVERITY_INFO);
	char text[MAX_LOG_LENGTH];
	if (command == MRD_COMMAND_REALLOC) {
		sprintf(text, "allocation (%d>%d) of ", realloc_src->id,
			current_allocation_id);

	} else {
		sprintf(text, "allocation (%d) of ", current_allocation_id);
	}

	MRL_log(text, MRL_SEVERITY_DEFAULT);

	if (command == MRD_COMMAND_REALLOC) {
		sprintf(text, "[%lu>%lu] ", realloc_src->size, size);
	} else {
		sprintf(text, "[%lu] ", size);
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

	MRS_String code_snippet;
	MRS_init_code_snippet(&code_snippet);

	MRD_get_code_snippet(file_name, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	MRL_log(text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	MRL_logln("", MRL_SEVERITY_DEFAULT);
}

void *MRD_malloc(size_t size, const char *file_name, int line)
{
	MRD_log_command(MRD_COMMAND_MALLOC, size, NULL, file_name, line);

#ifdef MRD_DEBUG_BACKTRACE
	MRD_log_backtrace();
#endif

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
		MRD_log_err("FAILED TO MALLOC ALLOCATE ");
	}

	MRL_logln("", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *MRD_calloc(size_t nmemb, size_t size, const char *file_name, int line)
{
	MRD_log_command(MRD_COMMAND_CALLOC, size, NULL, file_name, line);

#ifdef MRD_DEBUG_BACKTRACE
	MRD_log_backtrace();
#endif

	void *ptr = calloc(nmemb, size);
	if (ptr != NULL) {
		MRD_add_allocation_to_active_allocations(
			(struct Allocation){ .ptr = ptr,
					     .size = size,
					     .id = current_allocation_id,
					     .active = TRUE,
					     .reallocated_to = NULL });

	} else {
		MRD_log_err("FAILED TO CALLOC ALLOCATE");
	}

	MRL_logln("", MRL_SEVERITY_DEFAULT);

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

#ifdef MRD_DEBUG_BACKTRACE
	MRD_log_backtrace();
#endif

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
		MRD_log_err("FAILED TO REALLOCATE");
	}

	MRL_logln("", MRL_SEVERITY_DEFAULT);

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
		MRD_log_err("ATTEMPTED TO FREE NULL");
	} else {
		MRD_log_command(MRD_COMMAND_FREE, allocation->size, NULL,
				file_name, line);
	}

#ifdef MRD_DEBUG_BACKTRACE
	MRD_log_backtrace();
#endif

	MRL_logln("", MRL_SEVERITY_DEFAULT);

	free(ptr);
	allocation->active = FALSE;
}
