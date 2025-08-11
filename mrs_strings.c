#include "mrs_strings.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

MRS_String *MRS_create(size_t capacity)
{
	MRS_String *out = malloc(sizeof(*out));
	out->value = malloc(sizeof(char) * capacity);
	out->capacity = capacity;
	out->len = 0;
	return out;
}

MRS_String *MRS_init(size_t capacity, const char *value)
{
	if (capacity == 0) {
		capacity = strlen(value);
	}

	MRS_String *out = MRS_create(capacity);
	if (MRS_setstrn(out, value, strlen(value))) {
		MRS_free(out);
		return NULL;
	}
	return out;
}

void MRS_free(MRS_String *string)
{
	free(string->value);
	free(string);
}

void MRS_filter(MRS_String *string, const char remove_me)
{
	char filtered[string->len];
	size_t filtered_len = 0;
	for (size_t i = 0; i < string->len; i++) {
		if (string->value[i] != remove_me) {
			filtered[filtered_len] = string->value[i];
			filtered_len++;
		}
	}
	string->len = filtered_len;
	MRS_setstrn(string, filtered, filtered_len);
}

int MRS_setstr(MRS_String *string, const char *src)
{
	size_t len = strlen(src);
	if (len > string->capacity) {
		return 1;
	}

	for (size_t i = 0; i < len; i++) {
		string->value[i] = src[i];
	}
	string->len = len;

	return 0;
}

int MRS_setstrn(MRS_String *string, const char *src, size_t len)
{
	if (len > string->capacity) {
		return 1;
	}

	for (size_t i = 0; i < len; i++) {
		string->value[i] = src[i];
	}
	string->len = len;

	return 0;
}

int MRS_strcmp(MRS_String *a, MRS_String *b)
{
	if (a->len != b->len) {
		return 1;
	}

	for (size_t i = 0; i < a->len; i++) {
		if ((a->value[i] ^ b->value[i]) != 0) {
			return 1;
		}
	}

	return 0;
}

int MRS_strcat(MRS_String *dest, MRS_String *src)
{
	if (src->len + dest->len > dest->capacity) {
		return 1;
	}

	for (size_t i = 0; i < src->len; i++) {
		dest->value[dest->len] = src->value[i];
		dest->len++;
	}

	return 0;
}

char *MRS_strstr(MRS_String *haystack, MRS_String *needle,
		 size_t haystack_start_idx)
{
	for (size_t i = haystack_start_idx; haystack->len; i++) {
		for (size_t j = 0; needle->len; j++) {
			if ((haystack->value[i + j] ^ needle->value[j]) != 0) {
				break;
			}
			if (j == needle->len - 1) {
				return &haystack->value[i];
			}
		}
		if (i == haystack->len - needle->len) {
			break;
		}
	}
	return NULL;
}

char MRS_get_char(MRS_String *src, size_t idx)
{
	if (idx >= src->len) {
		return '\0';
	} else {
		return src->value[idx];
	}
}

int MRS_get_idx(MRS_String *src, char *idx, size_t *found_position)
{
	if (idx < src->value || idx >= &src->value[src->len]) {
		return -1;
	}
	*found_position = (int)(idx - src->value);
	return 0;
}

int MRS_is_whitespace(MRS_String *src, size_t idx)
{
	char c = MRS_get_char(src, idx);
	if (c == '\0') {
		return -1;
	}
	if (c == '\n' || c == '\t' || c == ' ') {
		return 0;
	}
	return 1;
}

void MRS_remove_whitespace(MRS_String *src)
{
	MRS_filter(src, ' ');
	MRS_filter(src, '\n');
	MRS_filter(src, '\t');
}

char *MRS_strchr(MRS_String *src, char target)
{
	for (size_t i = 0; i < src->len; i++) {
		if (src->value[i] == target) {
			return &src->value[i];
		}
	}
	return NULL;
}

MRS_String *MRS_strndup(MRS_String *src, size_t len)
{
	if (src->len < len) {
		return NULL;
	}
	MRS_String *out = MRS_create(len);
	MRS_setstrn(out, src->value, len);
	return out;
}
