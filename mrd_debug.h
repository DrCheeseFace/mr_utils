#ifndef MRD_DEBUG_H
#define MRD_DEBUG_H

#include <stdbool.h>
#include <stdlib.h>

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

#endif // !MRD_DEBUG_H
