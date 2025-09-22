#include "mrv_vectors.h"
#include "mrd_debug.h"
#include "mrm_misc.h"

#include <stdlib.h>
#include <string.h>

MrvVector *mrv_create(uint capacity, uint stride)
{
	MrvVector *vec = malloc(sizeof(MrvVector));

	vec->arr = malloc(capacity * stride);
	memset(vec->arr, CAFE_BABE, capacity * stride);

	vec->stride = stride;
	vec->len = 0;
	vec->capacity = capacity;

	return vec;
}

void mrv_destroy(MrvVector *vec)
{
	mrv_free(vec);
	free(vec);
}

void mrv_free(MrvVector *vec)
{
	if (vec != NULL && vec->arr != NULL) {
		free(vec->arr);
		vec->arr = NULL;
	}

	vec->stride = CAFE_BABE;
	vec->len = CAFE_BABE;
	vec->capacity = CAFE_BABE;
}

void mrv_init(MrvVector *vec, uint capacity, uint stride)
{
	vec->arr = malloc(capacity * stride);
	memset(vec->arr, CAFE_BABE, capacity * stride);
	vec->stride = stride;
	vec->len = 0;
	vec->capacity = capacity;

	return;
}

Err mrv_clear(MrvVector *vec)
{
	if (vec == NULL) {
		return ERR;
	}

	if (vec->arr == NULL) {
		return ERR;
	}

	memset(vec->arr, CAFE_BABE, vec->len * vec->stride);

	vec->len = 0;
	vec->capacity = 0;

	return OK;
}

Err mrv_append(MrvVector *vec, void *item)
{
	if (vec == NULL) {
		return ERR;
	}

	if (vec->capacity == vec->len) {
		vec->capacity++;
		vec->arr = realloc(vec->arr, vec->capacity * vec->stride);
	}

	memcpy(&vec->arr[vec->len * vec->stride], item, vec->stride);

	vec->len++;

	return OK;
}

Err mrv_realloc_to_fit(MrvVector *vec)
{
	if (vec == NULL) {
		return ERR;
	}

	if (vec->capacity == vec->len) {
		return OK;
	}

	vec->capacity = vec->len;
	vec->arr = realloc(vec->arr, vec->len * vec->stride);

	return OK;
}

Err mrv_pop(MrvVector *vec)
{
	if (vec == NULL) {
		return ERR;
	}

	if (vec->len == 0) {
		return ERR;
	}

	vec->len--;

	return OK;
}

void *mrv_get_idx(MrvVector *vec, uint n)
{
	if (vec == NULL) {
		return NULL;
	}

	if (n >= vec->len) {
		return NULL;
	}

	return &vec->arr[n * vec->stride];
}

void *mrv_get_item_where(MrvVector *vec, Bool (*is_item)(void *))
{
	for (uint i = 0; i < vec->len; i++) {
		if (is_item(&vec->arr[i * vec->stride]) == TRUE) {
			return &vec->arr[i * vec->stride];
		}
	}

	return NULL;
}

void *mrv_get_item(MrvVector *vec, void *item)
{
	for (uint i = 0; i < vec->len; i++) {
		if (memcmp(&vec->arr[i * vec->stride], item, vec->stride) ==
		    0) {
			return &vec->arr[i * vec->stride];
		}
	}

	return NULL;
}

void mrv_qsort(MrvVector *vec, int (*compare)(const void *, const void *))
{
	qsort(vec->arr, vec->len, vec->stride, compare);
}
