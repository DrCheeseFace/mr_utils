/*
 * this file is a string manipulation library
 */

#ifndef MRS_STRINGS_H
#define MRS_STRINGS_H
#include <mr_utils/mrm_misc.h>

/*
 * strings!
 *
 * MrsString has the following functions
 *
 * \ mrs_create                      - allocates a MRS_String on the heap and initializes it
 * \ mrs_init                        - initialize MRS_String
 * \ mrs_free                        - free contents of MRS_String
 * \ mrs_setstr                      - set MRS_String to str
 * \ mrs_setstrn                     - set n characters of str to MRS_String
 * \ mrs_strcmp                      - memcmp on contents of MRS_String
 * \ mrs_strcat                      - concatinates two MRS_String. reallocating if needed
 * \ mrs_pushstr                     - concatinates str to MRS_String. reallocating if needed
 * \ mrs_strstr                      - find MRS_String (needle) in MRS_String (haystack)
 * \ mrs_get_char                    - get char at index
 * \ mrs_get_idx                     - get index of first occurance of char
 * \ mrs_strchr                      - return pointer to first occurance of char
 * \ mrs_is_whitespace               - checks if characters at index is \t \n or ' '
 * \ mrs_remove_whitespace           - removes \t \n and ' '
 * \ mrs_filter                      - filters out char from MRS_String
 * \ mrs_strndup                     - return malloced copy of MRS_String
 * \ mrs_shrink_to_fit               - sets capacity to length and reallocates to fit
 * \ mrs_pop_char                    - removes last char
 * \ mrs_trim_trailing_whitespace    - removes trailing whitespace
 *
 */

typedef struct {
	char *value; // pointer to null terminated string
	uint len; // length of string. equivalent to the strlen()
	uint capacity; // characters malloced, not including null terminator
} MrsString;

MrsString *mrs_create(uint capacity);

/*
 * if `capactity = 0` capacity is set to strlen(value)
 * `returns` ERR if failed (double check capacity is enough)
 */
Err mrs_init(uint capacity, const char *value, uint value_len, MrsString *dest);

void mrs_free(MrsString *string);

Err mrs_setstr(MrsString *string, const char *src, uint src_len);

Err mrs_setstrn(MrsString *string, const char *src, uint src_len, uint len);

/*
 * returns -1 if a.len != b.len
 */
int mrs_strcmp(MrsString *a, MrsString *b);

Err mrs_strcat(MrsString *dest, MrsString *src);

/*
 * \ pushes n characters of str to dest
 * \ reallocs if capacity is exeeded
 */
Err mrs_pushstr(MrsString *dest, const char *str, uint n);

/*
 * `returns` NULL if not found
 */
char *mrs_strstr(MrsString *haystack, MrsString *needle,
		 uint haystack_start_idx);

/*
 * `returns` NULL terminator if out of bounds
 */
char mrs_get_char(MrsString *src, uint idx);

/*
 * populates `found_position` with idx
 *
 */
Err mrs_get_idx(MrsString *src, char *idx, uint *found_position);

/*
 * `returns` pointer to first occurance of char
 * `returns` NULL if not found
 */
char *mrs_strchr(MrsString *src, char target);

/*
 * checks for '\t' '\n' and ' '
 *
 * `returns` OK if true
 *
 * `returns` ERR if false
 *
 * `returns` NOT_FOUND if idx OOB
 */
Err mrs_is_whitespace(MrsString *src, uint idx);

void mrs_remove_whitespace(MrsString *src);

void mrs_filter(MrsString *string, const char remove_me);

Err mrs_strndup(MrsString *src, uint len, MrsString *dest);

/*
 * reallocs `value` to set `capacity` = `length`
 */
void mrs_shrink_to_fit(MrsString *src);

/*
 * removes last char
 *
 * `returns` ERR if src.len = 0
 */
Err mrs_pop_char(MrsString *src);

/*
 * removes trailing space
 */
void mrs_trim_trailing_whitespace(MrsString *src);

#endif // !MRS_STRINGS_H
