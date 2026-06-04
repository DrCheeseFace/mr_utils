#include <mr_utils.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

MrvVector *mrv_create(size_t capacity, size_t stride)
{
	MrvVector *vec = malloc(sizeof(MrvVector));
	if (vec == NULL)
		return NULL;

	vec->arr = malloc(capacity * stride);
	if (vec->arr == NULL) {
		free(vec);
		return NULL;
	}
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
	if (vec->arr != NULL) {
		free(vec->arr);
		vec->arr = NULL;
	}

	vec->stride = CAFE_BABE;
	vec->len = CAFE_BABE;
	vec->capacity = CAFE_BABE;
}

void *mrv_init(MrvVector *vec, size_t capacity, size_t stride)
{
	vec->arr = malloc(capacity * stride);
	if (vec->arr == NULL)
		return NULL;

	memset(vec->arr, CAFE_BABE, capacity * stride);
	vec->stride = stride;
	vec->len = 0;
	vec->capacity = capacity;

	return vec;
}

void *mrv_dupe(MrvVector *vec, MrvVector *dest)
{
	dest->stride = vec->stride;
	dest->len = vec->len;
	dest->capacity = vec->capacity;

	dest->arr = malloc(vec->capacity * vec->stride);
	if (dest->arr == NULL)
		return NULL;

	memcpy(dest->arr, vec->arr, vec->len * vec->stride);

	return dest;
}

void mrv_clear(MrvVector *vec)
{
	memset(vec->arr, CAFE_BABE, vec->len * vec->stride);

	vec->len = 0;
	vec->capacity = 0;
}

Err mrv_append(MrvVector *vec, void *item, Scaling scaling_method)
{
	size_t new_capacity = vec->capacity;

	if (vec->capacity == vec->len) {
		switch (scaling_method) {
		case APPEND_SCALING_INCREMENT:
			new_capacity++;
			break;
		case APPEND_SCALING_ONE_POINT_FIVE:
			if (new_capacity == 0) {
				new_capacity = 1;
			} else {
				new_capacity += (vec->capacity >> 1);
			}
			break;
		case APPEND_SCALING_DOUBLE:
			if (new_capacity == 0) {
				new_capacity = 1;
			} else {
				new_capacity <<= 1;
			}
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
	if (vec->capacity == vec->len) {
		return OK;
	}

	vec->capacity = vec->len;
	void *temp = realloc(vec->arr, vec->len * vec->stride);
	if (temp == NULL)
		return ERR;

	vec->arr = temp;
	return OK;
}

void mrv_pop(MrvVector *vec)
{
	vec->len--;
}

void mrv_pop_front(MrvVector *vec)
{
	memmove(vec->arr, vec->arr + vec->stride, (vec->len - 1) * vec->stride);
	vec->len--;
}

// TODO implement stable version
void mrv_remove(MrvVector *vec, size_t idx)
{
	void *item = mrv_get_idx(vec, idx);

	void *last = mrv_get_last(vec);

	if (item != last) {
		memcpy(item, last, vec->stride);
	}

	vec->len--;
}

void *mrv_get_idx(MrvVector *vec, size_t n)
{
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
