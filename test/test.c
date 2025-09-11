#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../mrd_debug.h"
#include "../mrs_strings.h"
#include "../mrt_test.h"
#include "../mrv_vectors.h"

int test_strstr(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_strstr");

	const char *haystack = "11151111111111111231";
	const char *needle = "31";

	MRS_String a;
	MRS_init(0, haystack, strlen(haystack), &a);

	MRS_String b;
	MRS_init(3, needle, strlen(needle), &b);

	struct MRT_Case test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 31",
		.pass = MRT_ASSERT_EQ(a.value[18], *MRS_strstr(&a, &b, 0))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	size_t start_idx = 3;
	test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 31 startidx=3",
		.pass = MRT_ASSERT_EQ(a.value[18],
				      *MRS_strstr(&a, &b, start_idx))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 31 startidx=NULL",
		.pass = MRT_ASSERT_EQ(a.value[18], *MRS_strstr(&a, &b, 0))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(&b, "52", strlen("52"));
	test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 52",
		.pass = MRT_ASSERT_NULL(MRS_strstr(&a, &b, 0))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(&b, "112", strlen("112"));
	test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 112",
		.pass = MRT_ASSERT_EQ(a.value[15], *MRS_strstr(&a, &b, 0))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(&b, "311", strlen("311"));
	test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 311 OOB",
		.pass = MRT_ASSERT_NULL(MRS_strstr(&a, &b, 0))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&a);
	MRS_free(&b);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_filter(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_filter");

	MRS_String actual;
	MRS_init(20, "12121", strlen("12121"), &actual);
	MRS_String expected;
	MRS_init(20, "111", strlen("111"), &expected);
	MRS_filter(&actual, '2');

	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "12121 | 2",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(&actual, "2222", strlen("2222"));
	MRS_setstr(&expected, "", strlen(""));
	MRS_filter(&actual, '2');
	test_case =
		(struct MRT_Case){ .description = "2222 | 2",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(&actual, "2222", strlen("2222"));
	MRS_setstr(&expected, "2222", strlen("2222"));
	MRS_filter(&actual, '3');
	test_case =
		(struct MRT_Case){ .description = "2222 | 3",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&actual);
	MRS_free(&expected);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_strcat(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_strcat");

	MRS_String actual;
	MRS_init(10, "aaabbb", strlen("aaabbb"), &actual);
	MRS_String append;
	MRS_init(10, "cc", strlen("cc"), &append);
	MRS_String expected;
	MRS_init(10, "aaabbbcc", strlen("aaabbbcc"), &expected);
	MRS_strcat(&actual, &append);

	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "aaabbb | cc",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(&actual, "123456789", strlen("123456789"));
	MRS_setstr(&append, "10", strlen("10"));
	MRS_setstr(&expected, "12345678910", strlen("12345678910"));
	test_case =
		(struct MRT_Case){ .description =
					   "123456789 | 10 over capacity",
				   .pass = 0 == MRS_strcat(&actual, &append) };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "123456789 | 10 over capacity length check",
		.pass = 11 == actual.capacity
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "123456789 | 10 over capacity equals",
		.pass = MRS_strcmp(&expected, &actual)
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&actual);
	MRS_free(&expected);
	MRS_init(9, "123456789", strlen("123456789"), &actual);
	MRS_init(10, "1234567891", strlen("1234567891"), &expected);
	MRS_setstr(&append, "1", strlen("1"));
	MRS_strcat(&actual, &append);
	test_case =
		(struct MRT_Case){ .description =
					   "123456789 | 1 exactly capacity",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(&actual, "123456789", strlen("123456789"));
	MRS_setstr(&append, "", strlen(""));
	MRS_setstr(&expected, "123456789", strlen("123456789"));
	MRS_strcat(&actual, &append);
	test_case =
		(struct MRT_Case){ .description = "123456789 | empty src",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&actual);
	MRS_free(&expected);
	MRS_init(0, "", strlen(""), &actual);
	MRS_init(0, "123", strlen("123"), &expected);
	MRS_setstr(&append, "123", strlen("123"));

	MRS_strcat(&actual, &append);
	test_case =
		(struct MRT_Case){ .description = "123456789 | empty dest",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(&actual, "", strlen(""));
	MRS_setstr(&append, "", strlen(""));
	MRS_setstr(&expected, "", strlen(""));
	MRS_strcat(&actual, &append);
	test_case =
		(struct MRT_Case){ .description = "empty dest and src",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&actual);
	MRS_free(&expected);
	MRS_free(&append);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_pushstr(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_pushstr");

	MRS_String actual;
	MRS_init(10, "aaabbb", strlen("aaabbb"), &actual);
	const char *append = "ccc";
	MRS_String expected;
	MRS_init(10, "aaabbbccc", strlen("aaabbbccc"), &expected);
	MRS_pushstr(&actual, append, strlen(append));

	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "aaabbb | ccc",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){ .description = "aaabbb | ccc capactity",
				       .pass = actual.capacity == 10 };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_pushstr(&actual, append, strlen(append));
	MRS_free(&expected);
	MRS_init(12, "aaabbbcccccc", strlen("aaabbbcccccc"), &expected);

	test_case =
		(struct MRT_Case){ .description = "aaabbbccc | ccc",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case =
		(struct MRT_Case){ .description = "aaabbbccc | ccc capacity",
				   .pass = actual.capacity == 12 };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_pushstr(&actual, "", strlen(""));

	test_case =
		(struct MRT_Case){ .description = "aaabbbcccccc | ",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){ .description = "aaabbbcccccc | ",
				       .pass = actual.capacity == 12 };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&actual);
	MRS_free(&expected);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_get_char(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_strcat");

	MRS_String src;
	MRS_init(10, "0123456789", strlen("0123456789"), &src);

	char actual = MRS_get_char(&src, 3);
	char expected = '3';
	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "within bounds fully",
				   .pass = MRT_ASSERT_EQ(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	actual = MRS_get_char(&src, 0);
	expected = '0';
	test_case =
		(struct MRT_Case){ .description = "zeroth idx",
				   .pass = MRT_ASSERT_EQ(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	actual = MRS_get_char(&src, 9);
	expected = '9';
	test_case =
		(struct MRT_Case){ .description = "last idx",
				   .pass = MRT_ASSERT_EQ(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	actual = MRS_get_char(&src, 10);
	expected = '\0';
	test_case =
		(struct MRT_Case){ .description = "outof bounds",
				   .pass = MRT_ASSERT_EQ(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&src);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_setstr(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_setstr");

	const char *from = "from";
	const char *to = "to";

	MRS_String actual;
	MRS_String expected;

	MRS_init(strlen(from), from, strlen(from), &actual);
	int result = MRS_setstr(&actual, to, strlen(to));
	MRS_init(strlen(to), to, strlen(to), &expected);

	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "from -> to",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){ .description = "from -> to result",
				       .pass = result == 0 };
	MRT_ctx_append_case(t_ctx, test_case);

	result = MRS_setstr(&actual, to, strlen(from) + 1);

	test_case = (struct MRT_Case){ .description = "from -> from+1 length",
				       .pass = result == 1 };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&actual);
	MRS_free(&expected);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_setstrn(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_setstrn");

	const char *from = "from";
	const char *to = "to";

	MRS_String actual;
	MRS_String expected;

	MRS_init(strlen(from), from, strlen(from), &actual);
	MRS_init(strlen(to), to, 1, &expected);

	int result = MRS_setstrn(&actual, to, strlen(to), 1);

	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "from -> to length 1",
				   .pass = !MRS_strcmp(&expected, &actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case =
		(struct MRT_Case){ .description = "from -> to length 1 result",
				   .pass = result == 0 };
	MRT_ctx_append_case(t_ctx, test_case);

	result = MRS_setstrn(&actual, to, strlen(to), 3);

	test_case =
		(struct MRT_Case){ .description = "from -> to length 3 result",
				   .pass = result == 1 };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&actual);
	MRS_free(&expected);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_get_idx(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_get_idx");

	const char *from = "from";
	const char *random = "random";

	MRS_String example;
	MRS_String random_example;

	MRS_init(strlen(random), random, strlen(random), &random_example);
	MRS_init(strlen(from), from, strlen(from), &example);

	size_t idx_found;
	int result = MRS_get_idx(&example, &example.value[2], &idx_found);

	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "'from' find from[2]",
				   .pass = idx_found == 2 };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case =
		(struct MRT_Case){ .description = "'from' find from[2] result",
				   .pass = result == 0 };
	MRT_ctx_append_case(t_ctx, test_case);

	result = MRS_get_idx(&example, &random_example.value[2], &idx_found);
	test_case =
		(struct MRT_Case){ .description =
					   "'from' find char* outside result",
				   .pass = result == -1 };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&example);
	MRS_free(&random_example);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_strchr(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_strchr");

	const char *xample_str = "xample_str";
	MRS_String xample;
	MRS_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	char *result = MRS_strchr(&xample, 'l');
	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "'xample_str' find 'l'",
				   .pass = *result == 'l' };
	MRT_ctx_append_case(t_ctx, test_case);

	result = MRS_strchr(&xample, 'r');
	test_case = (struct MRT_Case){ .description = "'xample_str' find 'r'",
				       .pass = *result == 'r' };
	MRT_ctx_append_case(t_ctx, test_case);

	result = MRS_strchr(&xample, 'z');
	test_case = (struct MRT_Case){ .description = "'xample_str' find 'x'",
				       .pass = MRT_ASSERT_NULL(result) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&xample);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_strndup(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_strndup");

	const char *xample_str = "xample_str";
	MRS_String xample;
	MRS_String xample_dup;

	MRS_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	int result = MRS_strndup(&xample, xample.len, &xample_dup);
	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "dup full string",
				   .pass = !MRS_strcmp(&xample, &xample_dup) };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){ .description = "dup full string result",
				       .pass = result == 0 };
	MRT_ctx_append_case(t_ctx, test_case);
	MRS_free(&xample_dup);

	result = MRS_strndup(&xample, xample.len - 1, &xample_dup);
	test_case =
		(struct MRT_Case){ .description = "dup string len - 1",
				   .pass = xample_dup.len == xample.len - 1 };
	MRT_ctx_append_case(t_ctx, test_case);
	test_case = (struct MRT_Case){
		.description = "dup string len - 1 value check",
		.pass = !strcmp("xample_st", xample_dup.value)
	};
	MRT_ctx_append_case(t_ctx, test_case);
	test_case =
		(struct MRT_Case){ .description = "dup string len - 1 result",
				   .pass = result == 0 };
	MRT_ctx_append_case(t_ctx, test_case);
	MRS_free(&xample_dup);

	result = MRS_strndup(&xample, xample.len + 1, &xample_dup);
	test_case = (struct MRT_Case){ .description = "dup string len + 1",
				       .pass = result == -1 };
	MRT_ctx_append_case(t_ctx, test_case);
	MRS_free(&xample_dup);

	MRS_free(&xample);
	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_shrink_to_fit(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_shrink_to_fit");

	const char *example_str = "example_str";
	MRS_String example;
	MRS_init(32, example_str, strlen(example_str), &example);

	MRS_shrink_to_fit(&example);
	struct MRT_Case test_case = (struct MRT_Case){
		.description = "example_str capacity 32 length check",
		.pass = MRT_ASSERT_EQ(example.capacity, example.len)
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "example_str capacity 32 value check",
		.pass = strcmp(example.value, example_str) == 0
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(&example);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_append(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_append");

	MRV_Vector *int_array = MRV_create(10, sizeof(int));
	int append_val = CAFE_BABE;
	MRV_append(int_array, &append_val);

	struct MRT_Case test_case = (struct MRT_Case){
		.description = "empty array append within capacity len",
		.pass = int_array->len == 1
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "empty array append within capacity capacity",
		.pass = int_array->capacity == 10
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "empty array append within capacity stride",
		.pass = int_array->stride == sizeof(int)
	};
	MRT_ctx_append_case(t_ctx, test_case);

	int *val = MRV_get_idx(int_array, 0);
	test_case = (struct MRT_Case){
		.description = "empty array append within capacity val",
		.pass = *val == append_val
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRV_destroy(int_array);

	int_array = MRV_create(10, sizeof(int));

	test_case = (struct MRT_Case){
		.description = "empty array append to capacity capacity",
		.pass = int_array->capacity == 10
	};
	MRT_ctx_append_case(t_ctx, test_case);

	for (size_t i = 0; i < 10; i++) {
		MRV_append(int_array, &i);
		val = MRV_get_idx(int_array, i);

		test_case = (struct MRT_Case){
			.description = "empty array append to capacity value",
			.pass = *val == (int)i
		};
		MRT_ctx_append_case(t_ctx, test_case);
	}

	test_case =
		(struct MRT_Case){ .description =
					   "empty array append to capacity len",
				   .pass = int_array->len == 10 };
	MRT_ctx_append_case(t_ctx, test_case);

	Error err = MRV_append(int_array, &append_val);
	val = MRV_get_idx(int_array, 10);

	test_case = (struct MRT_Case){
		.description = "empty array append over capacity OK",
		.pass = err == OK
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "empty array append over capacity value",
		.pass = *val == append_val
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "empty array append over capacity len",
		.pass = int_array->len == 11
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "empty array append over capacity capacity",
		.pass = int_array->capacity == 11
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRV_destroy(int_array);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_pop(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_strstr");

	MRV_Vector *int_array = MRV_create(10, sizeof(int));
	int append_val = CAFE_BABE;
	MRV_append(int_array, &append_val);
	MRV_append(int_array, &append_val);
	MRV_append(int_array, &append_val);
	MRV_pop(int_array);

	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "pop len > 0 len",
				   .pass = int_array->len == 2 };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){ .description = "pop len > 0 capacity",
				       .pass = int_array->capacity == 10 };
	MRT_ctx_append_case(t_ctx, test_case);

	MRV_pop(int_array);
	Error err = MRV_pop(int_array);

	test_case = (struct MRT_Case){ .description = "pop to len = 0 len",
				       .pass = int_array->len == 0 };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){ .description = "pop to len = 0 capacity",
				       .pass = int_array->capacity == 10 };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){ .description = "pop to len = 0 err",
				       .pass = err == OK };
	MRT_ctx_append_case(t_ctx, test_case);

	err = MRV_pop(int_array);
	test_case = (struct MRT_Case){ .description = "pop at len = 0 len",
				       .pass = int_array->len == 0 };
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){ .description = "pop at len = 0 err",
				       .pass = err == ERR };
	MRT_ctx_append_case(t_ctx, test_case);

	MRV_destroy(int_array);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
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
	struct MRT_Context *t_ctx = MRT_ctx_create("test_get_item_where");

	MRV_Vector *int_array = MRV_create(10, sizeof(int));
	int append_val = 1;
	MRV_append(int_array, &append_val);
	append_val++;
	MRV_append(int_array, &append_val);
	append_val++;
	MRV_append(int_array, &append_val);
	append_val++;

	void *two_ptr = MRV_get_idx(int_array, 1);

	struct MRT_Case test_case = (struct MRT_Case){
		.description = "get middle item",
		.pass = MRV_get_item_where(int_array, is_two) == two_ptr
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "item doesnt exist",
		.pass = MRV_get_item_where(int_array, always_false) == NULL
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRV_destroy(int_array);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_get_item(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_get_item");

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
	struct MRT_Case test_case = (struct MRT_Case){
		.description = "get middle item",
		.pass = MRV_get_item(int_array, &append_val) == two_ptr
	};
	MRT_ctx_append_case(t_ctx, test_case);

	append_val = CAFE_BABE;
	test_case =
		(struct MRT_Case){ .description = "item doesnt exist",
				   .pass = MRV_get_item(int_array,
							&append_val) == NULL };
	MRT_ctx_append_case(t_ctx, test_case);

	MRV_destroy(int_array);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
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
