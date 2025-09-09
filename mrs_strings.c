#include "mrs_strings.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

MRS_String *MRS_create(size_t capacity)
{
	MRS_String *out = malloc(sizeof(*out));
	out->value = malloc(sizeof(char) * (capacity + 1));
	out->value[capacity] = '\0';
	out->capacity = capacity;
	out->len = 0;
	return out;
}

int MRS_init(size_t capacity, const char *value, size_t value_len,
	     MRS_String *dest)
{
	if (capacity == 0) {
		capacity = strlen(value);
	}

	dest->value = malloc(sizeof(char) * (capacity + 1));
	if (dest->value == NULL) {
		dest->capacity = 0;
		dest->len = 0;
		return -1;
	}
	dest->value[capacity] = '\0';
	dest->capacity = capacity;

	if (MRS_setstrn(dest, value, value_len, value_len)) {
		MRS_free(dest);
		return -1;
	}
	dest->value[value_len] = '\0';
	return 0;
}

void MRS_free(MRS_String *string)
{
	if (string != NULL && string->value != NULL) {
		free(string->value);
		string->value = NULL;
		string->len = 0;
		string->capacity = 0;
	}
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
	MRS_setstrn(string, filtered, filtered_len, filtered_len);
}

int MRS_setstr(MRS_String *string, const char *src, size_t src_len)
{
	if (src_len > string->capacity) {
		return 1;
	}

	memcpy(string->value, src, src_len);
	string->len = src_len;
	string->value[src_len] = '\0';

	return 0;
}

int MRS_setstrn(MRS_String *string, const char *src, size_t src_len, size_t len)
{
	if (len > string->capacity) {
		return 1;
	}
	if (len > src_len) {
		return 1;
	}

	memcpy(string->value, src, len);
	string->len = len;
	string->value[len] = '\0';

	return 0;
}

int MRS_strcmp(MRS_String *a, MRS_String *b)
{
	if (a->len != b->len) {
		return 1;
	}

	return memcmp(a->value, b->value, sizeof(char) * a->len);
}

int MRS_strcat(MRS_String *dest, MRS_String *src)
{
	if (src->len + dest->len > dest->capacity) {
		char *malloced =
			malloc(sizeof(char) * (dest->len + src->len + 1));
		if (malloced == NULL) {
			return 1;
		}

		memcpy(malloced, dest->value, dest->len);
		memcpy(&malloced[dest->len], src->value, src->len);
		free(dest->value);

		dest->value = malloced;
		dest->capacity = src->len + dest->len;
		dest->len = src->len + dest->len;

		dest->value[dest->len] = '\0';
		return 0;
	}

	memcpy(&dest->value[dest->len], src->value, src->len);
	dest->len += src->len;
	dest->value[dest->len] = '\0';

	return 0;
}

int MRS_pushstr(MRS_String *dest, const char *append_me, size_t n)
{
	if (n + dest->len > dest->capacity) {
		char *malloced = malloc(sizeof(char) * (dest->len + n + 1));
		if (malloced == NULL) {
			return 1;
		}

		memcpy(malloced, dest->value, dest->len);
		memcpy(&malloced[dest->len], append_me, n);
		free(dest->value);

		dest->value = malloced;
		dest->capacity = n + dest->len;
		dest->len = n + dest->len;

		dest->value[dest->len] = '\0';
		return 0;
	}

	memcpy(&dest->value[dest->len], append_me, n);
	dest->len += n;
	dest->value[dest->len] = '\0';

	return 0;
}

char *MRS_strstr(MRS_String *haystack, MRS_String *needle,
		 size_t haystack_start_idx)
{
	if (haystack->len < needle->len) {
		return NULL;
	}
	for (size_t i = haystack_start_idx; i < haystack->len - needle->len + 1;
	     i++) {
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
	*found_position = (size_t)(idx - src->value);
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

int MRS_strndup(MRS_String *src, size_t len, MRS_String *dest)
{
	if (src->len < len) {
		return -1;
	}

	dest->value = malloc(sizeof(char) * (len + 1));
	if (dest->value == NULL) {
		dest->capacity = 0;
		dest->len = 0;
		return -1;
	}
	memcpy(dest->value, src->value, sizeof(char) * len);
	dest->capacity = src->capacity;
	dest->len = len;
	dest->value[len] = '\0';
	return 0;
}

void MRS_shrink_to_fit(MRS_String *src)
{
	if (src->len == src->capacity) {
		return;
	}

	char *shrunk_string_alloc = malloc(sizeof(char) * (src->len + 1));
	memmove(shrunk_string_alloc, src->value, src->len);

	free(src->value);

	src->value = shrunk_string_alloc;
	src->capacity = src->len;
	src->value[src->len] = '\0';
}
