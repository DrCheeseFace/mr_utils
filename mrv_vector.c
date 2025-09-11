#include "mrv_vector.h"
#include "mrd_debug.h"
#include "mrm_misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MRV_Vector *MRV_create(size_t capacity, size_t stride)
{
	MRV_Vector *vec = malloc(sizeof(MRV_Vector));

	vec->arr = malloc(capacity * stride);
	memset(vec->arr, CAFE_BABE, capacity * stride);

	vec->stride = stride;
	vec->len = 0;
	vec->capacity = capacity;

	return vec;
}

void MRV_destroy(MRV_Vector *vec)
{
	MRV_free(vec);
	free(vec);
}

void MRV_free(MRV_Vector *vec)
{
	if (vec != NULL && vec->arr != NULL) {
		free(vec->arr);
		vec->arr = NULL;
	}

	vec->stride = CAFE_BABE;
	vec->len = CAFE_BABE;
	vec->capacity = CAFE_BABE;
}

void MRV_init(MRV_Vector *vec, size_t capacity, size_t stride)
{
	vec->arr = malloc(capacity * stride);
	memset(vec->arr, CAFE_BABE, capacity * stride);
	vec->stride = stride;
	vec->len = 0;
	vec->capacity = capacity;

	return;
}

MRM_ErrorCode MRV_clear(MRV_Vector *vec)
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

MRM_ErrorCode MRV_append(MRV_Vector *vec, void *item)
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

MRM_ErrorCode MRV_realloc_to_fit(MRV_Vector *vec)
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

MRM_ErrorCode MRV_pop(MRV_Vector *vec)
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

void *MRV_get_idx(MRV_Vector *vec, size_t n)
{
	if (vec == NULL) {
		return NULL;
	}

	if (n >= vec->len) {
		return NULL;
	}

	return &vec->arr[n * vec->stride];
}
