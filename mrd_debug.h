#ifndef MRD_DEBUG_H
#define MRD_DEBUG_H

#include <stdbool.h>
#include <stdlib.h>

#define MAX_ACTIVE_ALLOCATIONS 4096
#define MAX_SNIPPET_LEN 128

struct Allocation {
	void *ptr;
	bool active;
	int id;
	struct Allocation *reallocated_to;
	size_t size;
};

void *MRD_malloc(size_t size_of, const char *file, int line);

void *MRD_calloc(size_t nmemb, size_t size, const char *file, int line);

void *MRD_realloc(void *ptr, size_t size, const char *file, int line);

void MRD_free(void *ptr, const char *file, int line);

#endif // !MRD_DEBUG_H

#ifdef DEBUG

#define malloc(size) MRD_malloc(size, __FILE__, __LINE__)
#define calloc(nmemb, size) MRD_calloc(nmemb, size, __FILE__, __LINE__)
#define realloc(ptr, size) MRD_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) MRD_free(ptr, __FILE__, __LINE__)

#else

#endif // !DEBUG
