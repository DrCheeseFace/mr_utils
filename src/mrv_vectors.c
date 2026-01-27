#include <mr_utils.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

MrvVector *mrv_create(size_t capacity, size_t stride)
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

void mrv_init(MrvVector *vec, size_t capacity, size_t stride)
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

Err mrv_append(MrvVector *vec, void *item, Scaling scaling_method)
{
	if (vec == NULL) {
		return ERR;
	}

	size_t new_capacity = vec->capacity;

	if (vec->capacity == vec->len) {
		switch (scaling_method) {
		case APPEND_SCALING_INCREMENT:
			new_capacity++;
			break;
		case APPEND_SCALING_ONE_POINT_FIVE:
			new_capacity += (vec->capacity >> 1);
			break;
		case APPEND_SCALING_DOUBLE:
			new_capacity <<= 1;
			break;
		default:
			break;
		}

		void *temp = realloc(vec->arr, new_capacity * vec->stride);
		if (temp == NULL)
			return ERR;
		vec->arr = temp;
		vec->capacity = new_capacity;
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

Err mrv_remove(MrvVector *vec, size_t idx)
{
	void *item = mrv_get_idx(vec, idx);
	if (!item) {
		return ERR;
	}

	void *last = mrv_get_last(vec);

	if (item != last) {
		memcpy(item, last, vec->stride);
	}

	vec->len--;

	return OK;
}

void *mrv_get_idx(MrvVector *vec, size_t n)
{
	if (vec == NULL) {
		return NULL;
	}

	if (n >= vec->len) {
		return NULL;
	}

	return &vec->arr[n * vec->stride];
}

size_t mrv_get_pos(MrvVector *vec, void *item)
{
	return ((uintptr_t)item - (uintptr_t)vec->arr) / vec->stride;
}

void *mrv_get_last(MrvVector *vec)
{
	return mrv_get_idx(vec, vec->len - 1);
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
