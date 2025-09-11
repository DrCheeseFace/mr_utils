#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../mrd_debug.h"
#include "../mrs_strings.h"
#include "../mrt_test.h"
#include "../mrv_vectors.h"

int test_strstr(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_strstr");

	const char *haystack = "11151111111111111231";
	const char *needle = "31";

	MRS_String a;
	MRS_init(0, haystack, strlen(haystack), &a);

	MRS_String b;
	MRS_init(3, needle, strlen(needle), &b);

	MRT_ctx_append_case(t_ctx, "11151111111111111231 | 31",
			    MRT_ASSERT_EQ(a.value[18], *MRS_strstr(&a, &b, 0)));

	size_t start_idx = 3;

	MRT_ctx_append_case(t_ctx, "11151111111111111231 | 31 startidx=3",
			    MRT_ASSERT_EQ(a.value[18],
					  *MRS_strstr(&a, &b, start_idx)));
	MRT_ctx_append_case(t_ctx, "11151111111111111231 | 31 startidx=NULL",
			    MRT_ASSERT_EQ(a.value[18], *MRS_strstr(&a, &b, 0)));

	MRS_setstr(&b, "52", strlen("52"));

	MRT_ctx_append_case(t_ctx, "11151111111111111231 | 52",
			    MRT_ASSERT_NULL(MRS_strstr(&a, &b, 0)));

	MRS_setstr(&b, "112", strlen("112"));

	MRT_ctx_append_case(t_ctx, "11151111111111111231 | 112",
			    MRT_ASSERT_EQ(a.value[15], *MRS_strstr(&a, &b, 0)));

	MRS_setstr(&b, "311", strlen("311"));

	MRT_ctx_append_case(t_ctx, "11151111111111111231 | 311 OOB",
			    MRT_ASSERT_NULL(MRS_strstr(&a, &b, 0)));

	MRS_free(&a);
	MRS_free(&b);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_filter(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_filter");

	MRS_String actual;
	MRS_init(20, "12121", strlen("12121"), &actual);
	MRS_String expected;
	MRS_init(20, "111", strlen("111"), &expected);
	MRS_filter(&actual, '2');

	MRT_ctx_append_case(t_ctx, "12121 | 2",
			    !MRS_strcmp(&expected, &actual));

	MRS_setstr(&actual, "2222", strlen("2222"));
	MRS_setstr(&expected, "", strlen(""));
	MRS_filter(&actual, '2');

	MRT_ctx_append_case(t_ctx, "2222 | 2", !MRS_strcmp(&expected, &actual));

	MRS_setstr(&actual, "2222", strlen("2222"));
	MRS_setstr(&expected, "2222", strlen("2222"));
	MRS_filter(&actual, '3');

	MRT_ctx_append_case(t_ctx, "2222 | 3", !MRS_strcmp(&expected, &actual));

	MRS_free(&actual);
	MRS_free(&expected);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_strcat(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_strcat");

	MRS_String actual;
	MRS_init(10, "aaabbb", strlen("aaabbb"), &actual);
	MRS_String append;
	MRS_init(10, "cc", strlen("cc"), &append);
	MRS_String expected;
	MRS_init(10, "aaabbbcc", strlen("aaabbbcc"), &expected);
	MRS_strcat(&actual, &append);

	MRT_ctx_append_case(t_ctx, "aaabbb | cc",
			    !MRS_strcmp(&expected, &actual));

	MRS_setstr(&actual, "123456789", strlen("123456789"));
	MRS_setstr(&append, "10", strlen("10"));
	MRS_setstr(&expected, "12345678910", strlen("12345678910"));

	MRT_ctx_append_case(t_ctx, "123456789 | 10 over capacity",
			    OK == MRS_strcat(&actual, &append));
	MRT_ctx_append_case(t_ctx, "123456789 | 10 over capacity length check",
			    11 == actual.capacity);
	MRT_ctx_append_case(t_ctx, "123456789 | 10 over capacity equals",
			    MRS_strcmp(&expected, &actual));

	MRS_free(&actual);
	MRS_free(&expected);
	MRS_init(9, "123456789", strlen("123456789"), &actual);
	MRS_init(10, "1234567891", strlen("1234567891"), &expected);
	MRS_setstr(&append, "1", strlen("1"));
	MRS_strcat(&actual, &append);

	MRT_ctx_append_case(t_ctx, "123456789 | 1 exactly capacity",
			    !MRS_strcmp(&expected, &actual));

	MRS_setstr(&actual, "123456789", strlen("123456789"));
	MRS_setstr(&append, "", strlen(""));
	MRS_setstr(&expected, "123456789", strlen("123456789"));
	MRS_strcat(&actual, &append);

	MRT_ctx_append_case(t_ctx, "123456789 | empty src",
			    !MRS_strcmp(&expected, &actual));

	MRS_free(&actual);
	MRS_free(&expected);
	MRS_init(0, "", strlen(""), &actual);
	MRS_init(0, "123", strlen("123"), &expected);
	MRS_setstr(&append, "123", strlen("123"));

	MRS_strcat(&actual, &append);

	MRT_ctx_append_case(t_ctx, "123456789 | empty dest",
			    !MRS_strcmp(&expected, &actual));

	MRS_setstr(&actual, "", strlen(""));
	MRS_setstr(&append, "", strlen(""));
	MRS_setstr(&expected, "", strlen(""));
	MRS_strcat(&actual, &append);

	MRT_ctx_append_case(t_ctx, "empty dest and src",
			    !MRS_strcmp(&expected, &actual));

	MRS_free(&actual);
	MRS_free(&expected);
	MRS_free(&append);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_pushstr(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_pushstr");

	MRS_String actual;
	MRS_init(10, "aaabbb", strlen("aaabbb"), &actual);
	const char *append = "ccc";
	MRS_String expected;
	MRS_init(10, "aaabbbccc", strlen("aaabbbccc"), &expected);
	MRS_pushstr(&actual, append, strlen(append));

	MRT_ctx_append_case(t_ctx, "aaabbb | ccc",
			    !MRS_strcmp(&expected, &actual));
	MRT_ctx_append_case(t_ctx, "aaabbb | ccc capactity",
			    actual.capacity == 10);

	MRS_pushstr(&actual, append, strlen(append));
	MRS_free(&expected);
	MRS_init(12, "aaabbbcccccc", strlen("aaabbbcccccc"), &expected);

	MRT_ctx_append_case(t_ctx, "aaabbbccc | ccc",
			    !MRS_strcmp(&expected, &actual));
	MRT_ctx_append_case(t_ctx, "aaabbbccc | ccc capacity",
			    actual.capacity == 12);

	MRS_pushstr(&actual, "", strlen(""));

	MRT_ctx_append_case(t_ctx, "aaabbbcccccc | ",
			    !MRS_strcmp(&expected, &actual));
	MRT_ctx_append_case(t_ctx, "aaabbbcccccc | ", actual.capacity == 12);

	MRS_free(&actual);
	MRS_free(&expected);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_get_char(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_strcat");

	MRS_String src;
	MRS_init(10, "0123456789", strlen("0123456789"), &src);

	char actual = MRS_get_char(&src, 3);
	char expected = '3';

	MRT_ctx_append_case(t_ctx, "within bounds fully",
			    MRT_ASSERT_EQ(expected, actual));

	actual = MRS_get_char(&src, 0);
	expected = '0';

	MRT_ctx_append_case(t_ctx, "zeroth idx",
			    MRT_ASSERT_EQ(expected, actual));

	actual = MRS_get_char(&src, 9);
	expected = '9';

	MRT_ctx_append_case(t_ctx, "last idx", MRT_ASSERT_EQ(expected, actual));

	actual = MRS_get_char(&src, 10);
	expected = '\0';

	MRT_ctx_append_case(t_ctx, "outof bounds",
			    MRT_ASSERT_EQ(expected, actual));

	MRS_free(&src);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_setstr(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_setstr");

	const char *from = "from";
	const char *to = "to";

	MRS_String actual;
	MRS_String expected;

	MRS_init(strlen(from), from, strlen(from), &actual);
	int result = MRS_setstr(&actual, to, strlen(to));
	MRS_init(strlen(to), to, strlen(to), &expected);

	MRT_ctx_append_case(t_ctx, "from -> to",
			    !MRS_strcmp(&expected, &actual));
	MRT_ctx_append_case(t_ctx, "from -> to result", result == OK);

	result = MRS_setstr(&actual, to, strlen(from) + 1);

	MRT_ctx_append_case(t_ctx, "from -> from+1 length", result == ERR);

	MRS_free(&actual);
	MRS_free(&expected);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_setstrn(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_setstrn");

	const char *from = "from";
	const char *to = "to";

	MRS_String actual;
	MRS_String expected;

	MRS_init(strlen(from), from, strlen(from), &actual);
	MRS_init(strlen(to), to, 1, &expected);

	int result = MRS_setstrn(&actual, to, strlen(to), 1);

	MRT_ctx_append_case(t_ctx, "from -> to length 1",
			    !MRS_strcmp(&expected, &actual));
	MRT_ctx_append_case(t_ctx, "from -> to length 1 result", result == OK);

	result = MRS_setstrn(&actual, to, strlen(to), 3);

	MRT_ctx_append_case(t_ctx, "from -> to length 3 result", result == ERR);

	MRS_free(&actual);
	MRS_free(&expected);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_get_idx(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_get_idx");

	const char *from = "from";
	const char *random = "random";

	MRS_String example;
	MRS_String random_example;

	MRS_init(strlen(random), random, strlen(random), &random_example);
	MRS_init(strlen(from), from, strlen(from), &example);

	size_t idx_found;
	int result = MRS_get_idx(&example, &example.value[2], &idx_found);

	MRT_ctx_append_case(t_ctx, "'from' find from[2]", idx_found == 2);
	MRT_ctx_append_case(t_ctx, "'from' find from[2] result", result == OK);

	result = MRS_get_idx(&example, &random_example.value[2], &idx_found);

	MRT_ctx_append_case(t_ctx, "'from' find char* outside result",
			    result == NOT_FOUND);

	MRS_free(&example);
	MRS_free(&random_example);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_strchr(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_strchr");

	const char *xample_str = "xample_str";
	MRS_String xample;
	MRS_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	char *result = MRS_strchr(&xample, 'l');

	MRT_ctx_append_case(t_ctx, "'xample_str' find 'l'", *result == 'l');

	result = MRS_strchr(&xample, 'r');

	MRT_ctx_append_case(t_ctx, "'xample_str' find 'r'", *result == 'r');

	result = MRS_strchr(&xample, 'z');

	MRT_ctx_append_case(t_ctx, "'xample_str' find 'x'",
			    MRT_ASSERT_NULL(result));

	MRS_free(&xample);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_strndup(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_strndup");

	const char *xample_str = "xample_str";
	MRS_String xample;
	MRS_String xample_dup;

	MRS_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	int result = MRS_strndup(&xample, xample.len, &xample_dup);

	MRT_ctx_append_case(t_ctx, "dup full string",
			    !MRS_strcmp(&xample, &xample_dup));
	MRT_ctx_append_case(t_ctx, "dup full string result", result == OK);

	MRS_free(&xample_dup);

	result = MRS_strndup(&xample, xample.len - 1, &xample_dup);

	MRT_ctx_append_case(t_ctx, "dup string len - 1",
			    xample_dup.len == xample.len - 1);
	MRT_ctx_append_case(t_ctx, "dup string len - 1 value check",
			    !strcmp("xample_st", xample_dup.value));
	MRT_ctx_append_case(t_ctx, "dup string len - 1 result", result == OK);

	MRS_free(&xample_dup);

	result = MRS_strndup(&xample, xample.len + 1, &xample_dup);

	MRT_ctx_append_case(t_ctx, "dup string len + 1", result == ERR);

	MRS_free(&xample_dup);

	MRS_free(&xample);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_shrink_to_fit(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_shrink_to_fit");

	const char *example_str = "example_str";
	MRS_String example;
	MRS_init(32, example_str, strlen(example_str), &example);

	MRS_shrink_to_fit(&example);

	MRT_ctx_append_case(t_ctx, "example_str capacity 32 length check",
			    MRT_ASSERT_EQ(example.capacity, example.len));
	MRT_ctx_append_case(t_ctx, "example_str capacity 32 value check",
			    strcmp(example.value, example_str) == 0);

	MRS_free(&example);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_append(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_append");

	MRV_Vector *int_array = MRV_create(10, sizeof(int));
	int append_val = CAFE_BABE;
	MRV_append(int_array, &append_val);

	MRT_ctx_append_case(t_ctx, "empty array append within capacity len",
			    int_array->len == 1);
	MRT_ctx_append_case(t_ctx,
			    "empty array append within capacity capacity",
			    int_array->capacity == 10);
	MRT_ctx_append_case(t_ctx, "empty array append within capacity stride",
			    int_array->stride == sizeof(int));

	int *val = MRV_get_idx(int_array, 0);

	MRT_ctx_append_case(t_ctx, "empty array append within capacity val",
			    *val == append_val);

	MRV_destroy(int_array);

	int_array = MRV_create(10, sizeof(int));

	MRT_ctx_append_case(t_ctx, "empty array append to capacity capacity",
			    int_array->capacity == 10);

	for (size_t i = 0; i < 10; i++) {
		MRV_append(int_array, &i);
		val = MRV_get_idx(int_array, i);

		MRT_ctx_append_case(t_ctx,
				    "empty array append to capacity value",
				    *val == (int)i);
	}

	MRT_ctx_append_case(t_ctx, "empty array append to capacity len",
			    int_array->len == 10);

	Err err = MRV_append(int_array, &append_val);
	val = MRV_get_idx(int_array, 10);

	MRT_ctx_append_case(t_ctx, "empty array append over capacity OK",
			    err == OK);
	MRT_ctx_append_case(t_ctx, "empty array append over capacity value",
			    *val == append_val);
	MRT_ctx_append_case(t_ctx, "empty array append over capacity len",
			    int_array->len == 11);
	MRT_ctx_append_case(t_ctx, "empty array append over capacity capacity",
			    int_array->capacity == 11);

	MRV_destroy(int_array);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_pop(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_strstr");

	MRV_Vector *int_array = MRV_create(10, sizeof(int));
	int append_val = CAFE_BABE;
	MRV_append(int_array, &append_val);
	MRV_append(int_array, &append_val);
	MRV_append(int_array, &append_val);
	MRV_pop(int_array);

	MRT_ctx_append_case(t_ctx, "pop len > 0 len", int_array->len == 2);
	MRT_ctx_append_case(t_ctx, "pop len > 0 capacity",
			    int_array->capacity == 10);

	MRV_pop(int_array);
	Err err = MRV_pop(int_array);

	MRT_ctx_append_case(t_ctx, "pop to len = 0 len", int_array->len == 0);
	MRT_ctx_append_case(t_ctx, "pop to len = 0 capacity",
			    int_array->capacity == 10);
	MRT_ctx_append_case(t_ctx, "pop to len = 0 err", err == OK);

	err = MRV_pop(int_array);

	MRT_ctx_append_case(t_ctx, "pop at len = 0 len", int_array->len == 0);
	MRT_ctx_append_case(t_ctx, "pop at len = 0 err", err == ERR);

	MRV_destroy(int_array);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

Bool is_two(void *x)
{
	return *(int *)x == 2;
}

Bool always_false(void *_)
{
	(void)_;
	return FALSE;
}

int test_get_item_where(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_get_item_where");

	MRV_Vector *int_array = MRV_create(10, sizeof(int));
	int append_val = 1;
	MRV_append(int_array, &append_val);
	append_val++;
	MRV_append(int_array, &append_val);
	append_val++;
	MRV_append(int_array, &append_val);
	append_val++;

	void *two_ptr = MRV_get_idx(int_array, 1);

	MRT_ctx_append_case(t_ctx, "get middle item",
			    MRV_get_item_where(int_array, is_two) == two_ptr);
	MRT_ctx_append_case(t_ctx, "item doesnt exist",
			    MRV_get_item_where(int_array, always_false) ==
				    NULL);

	MRV_destroy(int_array);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int test_get_item(void)
{
	MRT_Context *t_ctx = MRT_ctx_create("test_get_item");

	MRV_Vector *int_array = MRV_create(10, sizeof(int));
	int append_val = 1;
	MRV_append(int_array, &append_val);
	append_val++;
	MRV_append(int_array, &append_val);
	append_val++;
	MRV_append(int_array, &append_val);
	append_val++;

	void *two_ptr = MRV_get_idx(int_array, 1);

	append_val = 2;

	MRT_ctx_append_case(t_ctx, "get middle item",
			    MRV_get_item(int_array, &append_val) == two_ptr);

	append_val = CAFE_BABE;

	MRT_ctx_append_case(t_ctx, "item doesnt exist",
			    MRV_get_item(int_array, &append_val) == NULL);

	MRV_destroy(int_array);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_destroy(t_ctx);
	return failed;
}

int bruh(void)
{
	void *ptr = malloc(69);
	free(ptr);

	ptr = calloc(1, 69);
	ptr = realloc(ptr, 90);
	free(ptr);

	return 0;
}

int main(void)
{
	int err = 0;

	// mrs_strings
	err = err || test_strstr();
	err = err || test_filter();
	err = err || test_strcat();
	err = err || test_pushstr();
	err = err || test_get_char();
	err = err || test_setstr();
	err = err || test_setstrn();
	err = err || test_get_idx();
	err = err || test_strchr();
	err = err || test_strndup();
	err = err || test_shrink_to_fit();

	// mrv_vectors
	err = err || test_append();
	err = err || test_pop();
	err = err || test_get_item_where();
	err = err || test_get_item();

	// err = err || bruh();

	return err;
}
