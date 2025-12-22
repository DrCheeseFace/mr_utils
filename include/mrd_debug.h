/*
 * this is a tool used for memory debugging
 *
 * MRD_DEBUG_BACKTRACE
 *
 * MRD_DEBUG_DEFAULT
 *
 * MRD_DEBUG_ONLY_CALLED_AND_ERR
 *
 */

#ifndef MRD_DEBUG_H
#define MRD_DEBUG_H

#include <stddef.h>

#define MAX_ACTIVE_ALLOCATIONS 1024
#define MAX_SNIPPET_LEN 128
#define MAX_BACKTRACE_LENGTH 4
#define MAX_BACKTRACE_DEPTH_PRINTOUT 8
#define MAX_FUNC_NAME_LEN 64
#define BASE_ADDRESS_SIZE 12
#define DEBUG_LOG_HEAD "MRD_DEBUG LOG: "
#define _GNU_SOURCE

void mrd_log_dump_active_allocations_here(void);
void *mrd_inspect_allocation(size_t allocation_id);

void *mrd_malloc(size_t size, const char *file_name, int line);
void *mrd_calloc(size_t nmemb, size_t size, const char *file_name, int line);
void *mrd_realloc(void *ptr, size_t size, const char *file_name, int line);
void mrd_free(void *ptr, const char *file_name, int line);

#endif // !DEBUG

#ifdef DEBUG

#include <stdlib.h>

#define malloc(size) mrd_malloc(size, __FILE__, __LINE__)
#define calloc(nmemb, size) mrd_calloc(nmemb, size, __FILE__, __LINE__)
#define realloc(ptr, size) mrd_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) mrd_free(ptr, __FILE__, __LINE__)

#endif // !MRD_DEBUG_H
