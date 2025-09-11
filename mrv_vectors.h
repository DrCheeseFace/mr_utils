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
 * \ MRV_create            - allocates a MRV_Vector on the heap and initializes it
 * \ MRV_destroy           - frees contents of MRV_Vector and itself
 * \ MRV_free              - frees contents of MRV_Vector
 * \ MRV_init              - set arr to CAFE_BABE and length/capacity to 0
 * \ MRV_clear             - set arr to CAFE_BABE and length/capacity to 0
 * \ MRV_append            - appends element
 * \ MRV_realloc_to_fit    - sets capacity to length and reallocates to fit
 * \ MRV_pop               - removes last element
 * \ MRV_get_idx           - get pointer to idx
 * \ MRV_get_item          - get pointer to first memcmp identical items
 * \ MRV_get_item_where    - get pointer to item that satisfies user defined method
 * \ MRV_qsort             - quicksort
 *
 */
typedef struct {
	unsigned char *arr;
	size_t stride;
	size_t len;
	size_t capacity;
} MRV_Vector;

MRV_Vector *MRV_create(size_t capacity, size_t stride);

void MRV_destroy(MRV_Vector *vec);

void MRV_free(MRV_Vector *vec);

void MRV_init(MRV_Vector *vec, size_t capacity, size_t stride);

Err MRV_clear(MRV_Vector *vec);

// resizes vec->arr if needed
Err MRV_append(MRV_Vector *vec, void *item);

Err MRV_realloc_to_fit(MRV_Vector *vec);

Err MRV_pop(MRV_Vector *vec);

void *MRV_get_idx(MRV_Vector *vec, size_t n);

void *MRV_get_item(MRV_Vector *vec, void *item);

void *MRV_get_item_where(MRV_Vector *vec, Bool (*is_item)(void *));

void MRV_qsort(MRV_Vector *vec, int (*compare)(const void *, const void *));

#endif // !MRV_VECTORS_H
