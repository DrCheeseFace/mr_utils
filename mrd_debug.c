#include "mrd_debug.h"
#include "internals.h"
#include "mrl_logger.h"
#include "mrm_misc.h"
#include "mrs_strings.h"
#include <string.h>

#ifdef malloc
#undef malloc
#undef calloc
#undef realloc
#undef free
#endif

#include "execinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct MrdAllocation {
	void *ptr;
	Bool active;
	int id;
	struct MrdAllocation *reallocated_to;
	size_t size;
};

typedef enum MRD_Command {
	MRD_COMMAND_MALLOC,
	MRD_COMMAND_CALLOC,
	MRD_COMMAND_REALLOC,
	MRD_COMMAND_FREE,
} MrdCommand;

global_variable int current_allocation_id = 0;
global_variable struct MrdAllocation active_allocations[MAX_ACTIVE_ALLOCATIONS];
global_variable long base_address = CAFE_BABE;

struct MrlLogger logger = { .out = NULL,
			    .log_header_enabled = FALSE,
			    .terminal_color_enabled = TRUE };

internal void mrd_init_logger(void)
{
	logger.out = stderr;
}

// cant call MRS_init due to it calling malloc
internal void mrd_init_code_snippet(MrsString *dest)
{
	dest->value = malloc(sizeof(char) * (MAX_SNIPPET_LEN + 1));
	dest->value[MAX_SNIPPET_LEN] = '\0';
	dest->capacity = MAX_SNIPPET_LEN;
	dest->len = 0;
}

internal void mrd_log_err(const char *msg)
{
	mrl_log(&logger, DEBUG_LOG_HEAD, MRL_SEVERITY_INFO);
	mrl_logln(&logger, msg, MRL_SEVERITY_ERROR);
}

internal void mrd_get_code_snippet(const char *file_name, int line,
				   MrsString *dest)
{
	FILE *file = fopen(file_name, "r");

	MrsString read_buffer;
	mrd_init_code_snippet(&read_buffer);

	int current_line = 1;
	while (fgets(read_buffer.value, read_buffer.capacity, file)) {
		read_buffer.len = strlen(read_buffer.value);

		if (current_line == line) {
			break;
		}

		if (mrs_strchr(&read_buffer, '\n') != NULL) {
			current_line++;
		}
	}
	fclose(file);

	mrs_filter(&read_buffer, '\n');
	mrs_filter(&read_buffer, '\t');

	mrs_setstrn(dest, read_buffer.value, read_buffer.len, read_buffer.len);
	free(read_buffer.value);
	return;
}

internal void mrd_get_base_address(const char *path)
{
	int pid = getpid();
	char base_addr_string[BASE_ADDRESS_SIZE + 1];
	char command[256];
	sprintf(command,
		"cat /proc/%d/maps | grep %s | head -n 1 | awk '{print $1}' | cut -d'-' -f1",
		pid, path); // lol

	FILE *fp = popen(command, "r");
	char output_buffer[BASE_ADDRESS_SIZE] = "";
	// has newline and \0 hence the +2
	char output_full_out[BASE_ADDRESS_SIZE + 2] = "";
	while (fgets(output_buffer, sizeof(output_buffer), fp) != NULL) {
		strcat(output_full_out, output_buffer);
	}

	for (size_t i = 0; i < BASE_ADDRESS_SIZE; i++) {
		base_addr_string[i] = output_full_out[i];
	}

	long base_addr_long = strtol(base_addr_string, NULL, 16);

	base_address = base_addr_long;

	return;
}

internal void unused mrd_log_backtrace(void)
{
	void *buffer[MAX_BACKTRACE_LENGTH];
	int nptrs = backtrace(buffer, MAX_BACKTRACE_LENGTH);

	char **symbols = backtrace_symbols(buffer, nptrs);
	if (symbols == NULL) {
		mrd_log_err("FAILED TO GET BACKTRACE SYMBOLS");
	}

	// get path of exec
	char *end_of_path = strchr(symbols[1], '(');
	char path[128];
	strncpy(path, symbols[1], end_of_path - symbols[1]);
	path[end_of_path - symbols[1]] = '\0';

	if (base_address == CAFE_BABE) {
		mrd_get_base_address(path);
	}

	int max_backtrace_depth_printout = MAX_BACKTRACE_DEPTH_PRINTOUT;
	if (nptrs < max_backtrace_depth_printout) {
		max_backtrace_depth_printout = nptrs;
	}

	// 2 is used here to remove depth created in this file
	size_t indent_level = 0;
	for (size_t i = 2; i < (size_t)max_backtrace_depth_printout; i++) {
		// calc diff between base and call addr
		char call_addr[BASE_ADDRESS_SIZE + 1];
		call_addr[BASE_ADDRESS_SIZE] = '\0';
		char *open_bracket = strchr(symbols[i], '[');
		open_bracket += 3;
		strncpy(call_addr, open_bracket, BASE_ADDRESS_SIZE);

		long call_addr_long = strtol(call_addr, NULL, 16);
		long addr_diff = call_addr_long - base_address;
		char addr_diff_hex[32]; // eg: +0xCDCD
		sprintf(addr_diff_hex, " +0x%lX", addr_diff);

		// build addr2line command
		char addr2line_command[128] = "";
		strcat(addr2line_command, "addr2line -f -i -e ");
		strcat(addr2line_command, path);
		strcat(addr2line_command, " ");
		strcat(addr2line_command, addr_diff_hex);

		// exec addr2line command
		FILE *fp = popen(addr2line_command, "r");
		char addr2line_output_buffer[128] = "";
		char addr2line_output_full_out[128] = "";
		while (fgets(addr2line_output_buffer,
			     sizeof(addr2line_output_buffer), fp) != NULL) {
			strcat(addr2line_output_full_out,
			       addr2line_output_buffer);
		}

		char func_name[MAX_FUNC_NAME_LEN + 1] = "";
		func_name[MAX_FUNC_NAME_LEN] = '\0';

		char *newline_pos = strchr(addr2line_output_full_out, '\n');
		*newline_pos = '\0';
		strtok(addr2line_output_full_out, "\n");
		size_t func_name_len = newline_pos - addr2line_output_full_out;
		strncpy(func_name, addr2line_output_full_out, func_name_len);
		strtok(newline_pos + 1, "\n");

		char log[256];
		sprintf(log, "%s => %s()", newline_pos + 1, func_name);

		for (size_t j = 0; j < indent_level; j++) {
			mrl_log(&logger, "  ", MRL_SEVERITY_DEFAULT);
		}
		indent_level++;
		mrl_log(&logger, "↪ ", MRL_SEVERITY_DEFAULT);
		mrl_logln(&logger, log, MRL_SEVERITY_DEFAULT);
	}

	free(symbols);
}

// returns 1 if true
internal int mrd_is_free_active_allocation_slot(struct MrdAllocation allocation)
{
	if (allocation.active == FALSE) {
		if (allocation.reallocated_to == NULL) {
			return 1;
		}
		if (allocation.reallocated_to->active == FALSE) {
			return 1;
		}
	}
	return 0;
}

// populates first available slot
internal void
mrd_add_allocation_to_active_allocations(struct MrdAllocation new_allocation)
{
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (mrd_is_free_active_allocation_slot(active_allocations[i])) {
			active_allocations[i] = new_allocation;
			current_allocation_id++;
			return;
		}
	}
	char err[64];
	sprintf(err, "MAX_ACTIVE_ALLOCATIONS reached (%d)",
		MAX_ACTIVE_ALLOCATIONS);

	mrd_log_err(err);
}

internal void mrd_log_command(enum MRD_Command command, size_t size,
			      struct MrdAllocation *realloc_free_src,
			      const char *file_name, int line)
{
	mrl_log(&logger, DEBUG_LOG_HEAD, MRL_SEVERITY_INFO);
	char text[MAX_LOG_LENGTH];
	if (command == MRD_COMMAND_REALLOC) {
		sprintf(text, "allocation (%d>%d) of ", realloc_free_src->id,
			current_allocation_id);

	} else if (command == MRD_COMMAND_FREE) {
		sprintf(text, "allocation (%d) of ", realloc_free_src->id);
	} else {
		sprintf(text, "allocation (%d) of ", current_allocation_id);
	}

	mrl_log(&logger, text, MRL_SEVERITY_DEFAULT);

	if (command == MRD_COMMAND_REALLOC) {
		sprintf(text, "[%lu>%lu] ", realloc_free_src->size, size);
	} else {
		sprintf(text, "[%lu] ", size);
	}
	mrl_log(&logger, text, MRL_SEVERITY_OK);

	mrl_log(&logger, "bytes ", MRL_SEVERITY_DEFAULT);
	switch (command) {
	case MRD_COMMAND_MALLOC:
		mrl_log(&logger, "malloc allocated ", MRL_SEVERITY_INFO);
		break;
	case MRD_COMMAND_CALLOC:
		mrl_log(&logger, "calloc allocated ", MRL_SEVERITY_INFO);
		break;
	case MRD_COMMAND_REALLOC:
		mrl_log(&logger, "realloc allocated ", MRL_SEVERITY_ALT_INFO);
		break;
	case MRD_COMMAND_FREE:
		mrl_log(&logger, "free'd ", MRL_SEVERITY_WARNING);
		break;
	default:
		break;
	}

	mrl_log(&logger, "in ", MRL_SEVERITY_DEFAULT);
	sprintf(text, "%s:%d ", file_name, line);
	mrl_log(&logger, text, MRL_SEVERITY_OK);

	MrsString code_snippet;
	mrd_init_code_snippet(&code_snippet);

	mrd_get_code_snippet(file_name, line, &code_snippet);
	sprintf(text, "%s", code_snippet.value);
	mrl_log(&logger, text, MRL_SEVERITY_DEFAULT);
	free(code_snippet.value);

	mrl_logln(&logger, "", MRL_SEVERITY_DEFAULT);
}

void *mrd_malloc(size_t size, const char *file_name, int line)
{
	if (logger.out == NULL) {
		mrd_init_logger();
	}

	mrd_log_command(MRD_COMMAND_MALLOC, size, NULL, file_name, line);

#ifdef MRD_DEBUG_BACKTRACE
	mrd_log_backtrace();
#endif

	void *ptr = malloc(size);
	if (ptr != NULL) {
		memset(ptr, CAFE_BABE, size);
		mrd_add_allocation_to_active_allocations(
			(struct MrdAllocation){ .ptr = ptr,
						.size = size,
						.id = current_allocation_id,
						.active = TRUE,
						.reallocated_to = NULL });

	} else {
		mrd_log_err("FAILED TO MALLOC ALLOCATE ");
	}

	mrl_logln(&logger, "", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *mrd_calloc(size_t nmemb, size_t size, const char *file_name, int line)
{
	if (logger.out == NULL) {
		mrd_init_logger();
	}

	mrd_log_command(MRD_COMMAND_CALLOC, size, NULL, file_name, line);

#ifdef MRD_DEBUG_BACKTRACE
	mrd_log_backtrace();
#endif

	void *ptr = calloc(nmemb, size);
	if (ptr != NULL) {
		mrd_add_allocation_to_active_allocations(
			(struct MrdAllocation){ .ptr = ptr,
						.size = size,
						.id = current_allocation_id,
						.active = TRUE,
						.reallocated_to = NULL });

	} else {
		mrd_log_err("FAILED TO CALLOC ALLOCATE");
	}

	mrl_logln(&logger, "", MRL_SEVERITY_DEFAULT);

	return ptr;
}

void *mrd_realloc(void *ptr, size_t size, const char *file_name, int line)
{
	if (logger.out == NULL) {
		mrd_init_logger();
	}

	struct MrdAllocation *src_allocation = NULL;
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (ptr == active_allocations[i].ptr) {
			src_allocation = &active_allocations[i];
			break;
		}
	}

	mrd_log_command(MRD_COMMAND_REALLOC, size, src_allocation, file_name,
			line);

#ifdef MRD_DEBUG_BACKTRACE
	mrd_log_backtrace();
#endif

	void *realloc_ptr = realloc(ptr, size);
	if (realloc_ptr != NULL) {
		memset(realloc_ptr, CAFE_BABE, size);
		src_allocation->active = FALSE;
		src_allocation->reallocated_to = realloc_ptr;

		mrd_add_allocation_to_active_allocations(
			(struct MrdAllocation){ .ptr = realloc_ptr,
						.size = size,
						.id = current_allocation_id,
						.active = TRUE,
						.reallocated_to = NULL });

	} else {
		mrd_log_err("FAILED TO REALLOCATE");
	}

	mrl_logln(&logger, "", MRL_SEVERITY_DEFAULT);

	return realloc_ptr;
}

void mrd_free(void *ptr, const char *file_name, int line)
{
	if (logger.out == NULL) {
		mrd_init_logger();
	}

	struct MrdAllocation *allocation = NULL;
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (ptr == active_allocations[i].ptr) {
			allocation = &active_allocations[i];
			break;
		}
	}

	if (ptr == NULL) {
		mrd_log_err("ATTEMPTED TO FREE NULL");
	} else {
		mrd_log_command(MRD_COMMAND_FREE, allocation->size, allocation,
				file_name, line);
	}

#ifdef MRD_DEBUG_BACKTRACE
	mrd_log_backtrace();
#endif

	mrl_logln(&logger, "", MRL_SEVERITY_DEFAULT);

	// if the pointer to free is ever realloced somewhere, we need to set this to NULL
	for (size_t i = 0; i < MAX_ACTIVE_ALLOCATIONS; i++) {
		if (allocation->ptr == active_allocations[i].reallocated_to) {
			active_allocations[i].reallocated_to = NULL;
			break;
		}
	}

	free(ptr);

	allocation->active = FALSE;
}
