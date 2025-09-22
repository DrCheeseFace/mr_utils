/*
 * this file is a vector library
 */

#ifndef MRV_VECTORS_H
#define MRV_VECTORS_H

#include "mrm_misc.h"
#include <stdlib.h>

/*
 * vectors!
 *
 * MrvVector has the following functions
 *
 * \ mrv_create            - allocates a MRV_Vector on the heap and initializes it
 * \ mrv_destroy           - frees contents of MRV_Vector and itself
 * \ mrv_free              - frees contents of MRV_Vector
 * \ mrv_init              - set arr to CAFE_BABE and length/capacity to 0
 * \ mrv_clear             - set arr to CAFE_BABE and length/capacity to 0
 * \ mrv_append            - appends element
 * \ mrv_realloc_to_fit    - sets capacity to length and reallocates to fit
 * \ mrv_pop               - removes last element
 * \ mrv_get_idx           - get pointer to idx
 * \ mrv_get_item          - get pointer to first memcmp identical items
 * \ mrv_get_item_where    - get pointer to item that satisfies user defined method
 * \ mrv_qsort             - quicksort
 *
 */

typedef struct {
	unsigned char *arr;
	size_t stride;
	size_t len;
	size_t capacity;
} MrvVector;

MrvVector *mrv_create(size_t capacity, size_t stride);

void mrv_destroy(MrvVector *vec);

void mrv_free(MrvVector *vec);

void mrv_init(MrvVector *vec, size_t capacity, size_t stride);

Err mrv_clear(MrvVector *vec);

// resizes vec->arr if needed
Err mrv_append(MrvVector *vec, void *item);

Err mrv_realloc_to_fit(MrvVector *vec);

Err mrv_pop(MrvVector *vec);

void *mrv_get_idx(MrvVector *vec, size_t n);

void *mrv_get_item(MrvVector *vec, void *item);

void *mrv_get_item_where(MrvVector *vec, Bool (*is_item)(void *));

void mrv_qsort(MrvVector *vec, int (*compare)(const void *, const void *));

#endif // !MRV_VECTORS_H
