/*
 * this is a tool used for memory debugging
 */

#ifndef MRD_DEBUG_H
#define MRD_DEBUG_H

#include "../mrm_misc.h"
#include <stddef.h>

#define MAX_ACTIVE_ALLOCATIONS 1024
#define MAX_SNIPPET_LEN 128
#define MAX_LOG_LENGTH 512

/*
 * Define for log levels 
 *
 * MRD_LOG_LEVEL_ALL
 * MRD_LOG_LEVEL_NORMAL
 * MRD_LOG_LEVEL_ERR_ONLY
 *
 */

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

#endif // !MRD_DEBUG_H
