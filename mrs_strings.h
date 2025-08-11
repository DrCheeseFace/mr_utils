#ifndef MRS_STRINGS_H
#define MRS_STRINGS_H
#include <stddef.h>
#include <string.h>

#define MRS_MAX_STRING_LENGTH 100

typedef struct {
	char *value;
	size_t len;
	size_t capacity;
} MRS_String;

MRS_String *MRS_create(size_t capacity);

/*
 * if `capactity = 0` capacity is set to strlen(value)
 * `returns` NULL if failed (double check capacity is enough)
 */
MRS_String *MRS_init(size_t capacity, const char *value);

void MRS_free(MRS_String *string);

/*
 * `returns` 0 if successful
 */
int MRS_setstr(MRS_String *string, const char *src);

/*
 * `returns` 0 if successful
 */
int MRS_setstrn(MRS_String *string, const char *src, size_t len);

/*
 * `returns` 0 if successful
 */
int MRS_strcmp(MRS_String *a, MRS_String *b);

/*
 * `returns` 0 if successful
 */
int MRS_strcat(MRS_String *dest, MRS_String *src);

/*
 * `returns` NULL if not found
 */
char *MRS_strstr(MRS_String *haystack, MRS_String *needle,
		 size_t haystack_start_idx);

/*
 * `returns` NULL terminator if out of bounds
 */
char MRS_get_char(MRS_String *src, size_t idx);

/*
 * populates `found_position` with idx
 * 
 * `returns` -1 if not found
 * `returns` 0 if successful 
 */
int MRS_get_idx(MRS_String *src, char *idx, size_t *found_position);

/*
 * `returns` pointer to first occurance of char
 * `returns` NULL if not found
 */
char *MRS_strchr(MRS_String *src, char target);

/*
 * checks for '\t' '\n' and ' '
 *
 * `returns` 0 is true
 *
 * `returns` 1 if false 
 *
 * `returns` -1 if idx OOB
 */
int MRS_is_whitespace(MRS_String *src, size_t idx);

void MRS_remove_whitespace(MRS_String *src);

void MRS_filter(MRS_String *string, const char remove_me);

/*
 * `returns` NULL if len < src.len
 */
MRS_String *MRS_strndup(MRS_String *src, size_t len);

#endif // !MRS_STRINGS_H
