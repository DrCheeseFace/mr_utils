#include <mr_utils.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

MrsString *mrs_create(uint capacity)
{
	MrsString *out = malloc(sizeof(*out));
	out->value = malloc(sizeof(char) * (capacity + 1));
	out->value[capacity] = '\0';
	out->capacity = capacity;
	out->len = 0;
	return out;
}

Err mrs_init(uint capacity, const char *value, uint value_len, MrsString *dest)
{
	if (capacity == 0) {
		capacity = strlen(value);
	}

	dest->value = malloc(sizeof(char) * (capacity + 1));
	if (dest->value == NULL) {
		dest->capacity = 0;
		dest->len = 0;
		return ERR;
	}
	dest->value[capacity] = '\0';
	dest->capacity = capacity;

	if (mrs_setstrn(dest, value, value_len, value_len)) {
		mrs_free(dest);
		return ERR;
	}
	dest->value[value_len] = '\0';
	return OK;
}

void mrs_free(MrsString *string)
{
	if (string != NULL && string->value != NULL) {
		free(string->value);
		string->value = NULL;
	}
	string->len = CAFE_BABE;
	string->capacity = CAFE_BABE;
}

void mrs_filter(MrsString *string, const char remove_me)
{
	char filtered[string->len];
	uint filtered_len = 0;
	for (uint i = 0; i < string->len; i++) {
		if (string->value[i] != remove_me) {
			filtered[filtered_len] = string->value[i];
			filtered_len++;
		}
	}
	mrs_setstrn(string, filtered, filtered_len, filtered_len);
}

Err mrs_setstr(MrsString *string, const char *src, uint src_len)
{
	if (src_len > string->capacity) {
		return ERR;
	}

	memcpy(string->value, src, src_len);
	string->len = src_len;
	string->value[src_len] = '\0';

	return OK;
}

Err mrs_setstrn(MrsString *string, const char *src, uint src_len, uint len)
{
	if (len > string->capacity) {
		return ERR;
	}
	if (len > src_len) {
		return ERR;
	}

	memcpy(string->value, src, len);
	string->len = len;
	string->value[len] = '\0';

	return OK;
}

int mrs_strcmp(MrsString *a, MrsString *b)
{
	if (a->len != b->len) {
		return -1;
	}

	return memcmp(a->value, b->value, sizeof(char) * a->len);
}

Err mrs_strcat(MrsString *dest, MrsString *src)
{
	if (src->len + dest->len > dest->capacity) {
		char *malloced =
			malloc(sizeof(char) * (dest->len + src->len + 1));
		if (malloced == NULL) {
			return ERR;
		}

		memcpy(malloced, dest->value, dest->len);
		memcpy(&malloced[dest->len], src->value, src->len);
		free(dest->value);

		dest->value = malloced;
		dest->capacity = src->len + dest->len;
		dest->len = src->len + dest->len;

		dest->value[dest->len] = '\0';
		return OK;
	}

	memcpy(&dest->value[dest->len], src->value, src->len);
	dest->len += src->len;
	dest->value[dest->len] = '\0';

	return OK;
}

Err mrs_pushstr(MrsString *dest, const char *append_me, uint n)
{
	uint len = dest->len + n;
	uint capacity = dest->capacity;
	if (len > dest->capacity) {
		capacity = len;
	}

	char *realloced = realloc(dest->value, sizeof(char) * (len + 1));
	if (realloced == NULL) {
		return ERR;
	}

	snprintf(&dest->value[dest->len], n + 1, "%s", append_me);

	dest->value = realloced;
	dest->capacity = capacity;
	dest->len = len;

	return OK;
}

char *mrs_strstr(MrsString *haystack, MrsString *needle,
		 uint haystack_start_idx)
{
	if (haystack->len < needle->len) {
		return NULL;
	}
	for (uint i = haystack_start_idx; i < haystack->len - needle->len + 1;
	     i++) {
		for (uint j = 0; needle->len; j++) {
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

char mrs_get_char(MrsString *src, uint idx)
{
	if (idx >= src->len) {
		return '\0';
	} else {
		return src->value[idx];
	}
}

Err mrs_get_idx(MrsString *src, char *idx, uint *found_position)
{
	if (idx < src->value || idx >= &src->value[src->len]) {
		return NOT_FOUND;
	}
	*found_position = (uint)(idx - src->value);
	return OK;
}

Err mrs_is_whitespace(MrsString *src, uint idx)
{
	char c = mrs_get_char(src, idx);
	if (c == '\0') {
		return NOT_FOUND;
	}
	if (c == '\n' || c == '\t' || c == ' ') {
		return OK;
	}
	return ERR;
}

void mrs_remove_whitespace(MrsString *src)
{
	mrs_filter(src, ' ');
	mrs_filter(src, '\n');
	mrs_filter(src, '\t');
}

char *mrs_strchr(MrsString *src, char target)
{
	for (uint i = 0; i < src->len; i++) {
		if (src->value[i] == target) {
			return &src->value[i];
		}
	}
	return NULL;
}

Err mrs_strndup(MrsString *src, uint len, MrsString *dest)
{
	if (src->len < len) {
		return ERR;
	}

	dest->value = malloc(sizeof(char) * (len + 1));
	if (dest->value == NULL) {
		dest->capacity = 0;
		dest->len = 0;
		return ERR;
	}
	memcpy(dest->value, src->value, sizeof(char) * len);
	dest->capacity = src->capacity;
	dest->len = len;
	dest->value[len] = '\0';
	return OK;
}

void mrs_shrink_to_fit(MrsString *src)
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

Err mrs_pop_char(MrsString *src)
{
	if (src->len == 0) {
		return ERR;
	}
	src->len--;
	src->value[src->len] = '\0';
	return OK;
}

void mrs_trim_trailing_whitespace(MrsString *src)
{
	while (TRUE) {
		if (src->len == 0) {
			return;
		}
		if (mrs_is_whitespace(src, src->len - 1) != OK) {
			return;
		}
		mrs_pop_char(src);
	}
}
