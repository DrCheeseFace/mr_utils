/*
 * this file is a vector library
 */

#ifndef MRV_VECTORS_H
#define MRV_VECTORS_H

#include "mrm_misc.h"
#include <stdlib.h>

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

MRM_ErrorCode MRV_clear(MRV_Vector *vec);

// resizes vec->arr if needed
MRM_ErrorCode MRV_append(MRV_Vector *vec, void *item);

MRM_ErrorCode MRV_realloc_to_fit(MRV_Vector *vec);

MRM_ErrorCode MRV_pop(MRV_Vector *vec);

void *MRV_get_idx(MRV_Vector *vec, size_t n);

void *MRV_get_item(MRV_Vector *vec, void *item);

void *MRV_get_item_where(MRV_Vector *vec, bool (*is_item)(void *));

#define MRV_qsort(vec, compare) qsort(vec->arr, vec->len, vec->stride, compare)

#endif // !MRV_VECTORS_H
