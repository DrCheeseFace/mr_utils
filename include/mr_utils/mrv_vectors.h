/*
 * this file is a vector library
 */

#ifndef MRV_VECTORS_H
#define MRV_VECTORS_H

#include <mr_utils/mrm_misc.h>
#include <stddef.h>

/*
 * vectors!
 *
 * MrvVector has the following functions
 *
 * \ mrv_create            - allocates a MRV_Vector on the heap and initializes it
 * \ mrv_destroy           - frees contents of MRV_Vector and itself
 * \ mrv_free              - frees contents of MRV_Vector
 * \ mrv_init              - set arr to CAFE_BABE and length/capacity to 0
 * \ mrv_dupe              - duplicates MrvVector. allocates to capacity
 * \ mrv_clear             - set arr to CAFE_BABE and length/capacity to 0
 * \ mrv_append            - appends element
 * \ mrv_realloc_to_fit    - sets capacity to length and reallocates to fit
 * \ mrv_pop               - removes last element
 * \ mrv_pop_front         - removed first element
 * \ mrv_remove            - removes array[index]'th element. uses swap and pop method
 * \ mrv_get_idx           - get pointer from idx
 * \ mrv_get_pos           - get idx from pointer
 * \ mrv_get_last          - get pointer to last item
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

typedef enum {
	APPEND_SCALING_INCREMENT,
	APPEND_SCALING_DOUBLE,
	APPEND_SCALING_ONE_POINT_FIVE,
	// APPEND_SCALING_PAGE_ALIGNED // TODO IMPLEMENT
} Scaling;

MrvVector *mrv_create(size_t capacity, size_t stride) WARN_UNUSED;

void mrv_destroy(MrvVector *vec) NONNULL_ARGS(1);

void mrv_free(MrvVector *vec) NONNULL_ARGS(1);

void *mrv_init(MrvVector *vec, size_t capacity, size_t stride) NONNULL_ARGS(1);

void *mrv_dupe(MrvVector *vec, MrvVector *dest) NONNULL_ARGS(1, 2);

void mrv_clear(MrvVector *vec) NONNULL_ARGS(1);

// resizes vec->arr if needed
// Scaling methods:
// 	APPEND_SCALING_INCREMENT,
// 	APPEND_SCALING_DOUBLE,
// 	APPEND_SCALING_ONE_POINT_FIVE,
// 	APPEND_SCALING_POWER_OF_TWO,
Err mrv_append(MrvVector *vec, void *item, Scaling scaling_method)
	NONNULL_ARGS(1, 2);

Err mrv_realloc_to_fit(MrvVector *vec) NONNULL_ARGS(1);

void mrv_pop(MrvVector *vec) NONNULL_ARGS(1);

void mrv_pop_front(MrvVector *vec) NONNULL_ARGS(1);

void mrv_remove(MrvVector *vec, size_t idx) NONNULL_ARGS(1);

void *mrv_get_idx(MrvVector *vec, size_t n) NONNULL_ARGS(1);

size_t mrv_get_pos(MrvVector *vec, void *item) NONNULL_ARGS(1);

void *mrv_get_last(MrvVector *vec) NONNULL_ARGS(1);

void *mrv_get_item(MrvVector *vec, void *item) NONNULL_ARGS(1);

void *mrv_get_item_where(MrvVector *vec, Bool (*is_item)(void *))
	NONNULL_ARGS(1, 2);

void mrv_qsort(MrvVector *vec, int (*compare)(const void *, const void *))
	NONNULL_ARGS(1, 2);

#endif // !MRV_VECTORS_H
