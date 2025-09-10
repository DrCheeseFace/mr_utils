/*
 * this is a tool used for memory debugging
 * 
 * MRD_DEBUG_BACKTRACE
 *
 * MRD_DEBUG_DEFAULT
 */

#ifndef MRD_DEBUG_H
#define MRD_DEBUG_H

#include "mrm_misc.h"
#include <stdlib.h>

#define MAX_ACTIVE_ALLOCATIONS 1024
#define MAX_SNIPPET_LEN 128
#define MAX_LOG_LENGTH 512
#define MAX_BACKTRACE_LENGTH 4
#define MAX_BACKTRACE_DEPTH_PRINTOUT 8
#define BASE_ADDRESS_SIZE 12
#define DEBUG_LOG_HEAD "MRD_DEBUG LOG: "

struct Allocation {
	void *ptr;
	bool active;
	int id;
	struct Allocation *reallocated_to;
	size_t size;
};

typedef enum MRD_Command {
	MRD_COMMAND_MALLOC,
	MRD_COMMAND_CALLOC,
	MRD_COMMAND_REALLOC,
	MRD_COMMAND_FREE,
} MRD_command;

void *MRD_malloc(size_t size, const char *file_name, int line);
void *MRD_calloc(size_t nmemb, size_t size, const char *file_name, int line);
void *MRD_realloc(void *ptr, size_t size, const char *file_name, int line);
void MRD_free(void *ptr, const char *file_name, int line);

#endif // !DEBUG

#ifdef DEBUG

#define malloc(size) MRD_malloc(size, __FILE__, __LINE__)
#define calloc(nmemb, size) MRD_calloc(nmemb, size, __FILE__, __LINE__)
#define realloc(ptr, size) MRD_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) MRD_free(ptr, __FILE__, __LINE__)

#endif // !MRD_DEBUG_H
