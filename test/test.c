#include <stdlib.h>
#include <string.h>

#include "../mrd_debug.h"
#include "../mrl_logger.h"
#include "../mrs_strings.h"
#include "../mrt_test.h"
#include "../mrv_vectors.h"

void test_strstr(MrtGroup *t_ctx)
{
	const char *haystack = "11151111111111111231";
	const char *needle = "31";

	MrsString a;
	mrs_init(0, haystack, strlen(haystack), &a);

	MrsString b;
	mrs_init(3, needle, strlen(needle), &b);

	mrt_group_append_case(t_ctx, "11151111111111111231 | 31",
			      MRT_ASSERT_EQ(a.value[18],
					    *mrs_strstr(&a, &b, 0)));

	size_t start_idx = 3;

	mrt_group_append_case(t_ctx, "11151111111111111231 | 31 startidx=3",
			      MRT_ASSERT_EQ(a.value[18],
					    *mrs_strstr(&a, &b, start_idx)));
	mrt_group_append_case(t_ctx, "11151111111111111231 | 31 startidx=NULL",
			      MRT_ASSERT_EQ(a.value[18],
					    *mrs_strstr(&a, &b, 0)));

	mrs_setstr(&b, "52", strlen("52"));

	mrt_group_append_case(t_ctx, "11151111111111111231 | 52",
			      MRT_ASSERT_NULL(mrs_strstr(&a, &b, 0)));

	mrs_setstr(&b, "112", strlen("112"));

	mrt_group_append_case(t_ctx, "11151111111111111231 | 112",
			      MRT_ASSERT_EQ(a.value[15],
					    *mrs_strstr(&a, &b, 0)));

	mrs_setstr(&b, "311", strlen("311"));

	mrt_group_append_case(t_ctx, "11151111111111111231 | 311 OOB",
			      MRT_ASSERT_NULL(mrs_strstr(&a, &b, 0)));

	mrs_free(&a);
	mrs_free(&b);
	return;
}

void test_filter(MrtGroup *t_ctx)
{
	MrsString actual;
	mrs_init(20, "12121", strlen("12121"), &actual);
	MrsString expected;
	mrs_init(20, "111", strlen("111"), &expected);
	mrs_filter(&actual, '2');

	mrt_group_append_case(t_ctx, "12121 | 2",
			      !mrs_strcmp(&expected, &actual));

	mrs_setstr(&actual, "2222", strlen("2222"));
	mrs_setstr(&expected, "", strlen(""));
	mrs_filter(&actual, '2');

	mrt_group_append_case(t_ctx, "2222 | 2",
			      !mrs_strcmp(&expected, &actual));

	mrs_setstr(&actual, "2222", strlen("2222"));
	mrs_setstr(&expected, "2222", strlen("2222"));
	mrs_filter(&actual, '3');

	mrt_group_append_case(t_ctx, "2222 | 3",
			      !mrs_strcmp(&expected, &actual));

	mrs_free(&actual);
	mrs_free(&expected);
}

void test_strcat(MrtGroup *t_ctx)
{
	MrsString actual;
	mrs_init(10, "aaabbb", strlen("aaabbb"), &actual);
	MrsString append;
	mrs_init(10, "cc", strlen("cc"), &append);
	MrsString expected;
	mrs_init(10, "aaabbbcc", strlen("aaabbbcc"), &expected);
	mrs_strcat(&actual, &append);

	mrt_group_append_case(t_ctx, "aaabbb | cc",
			      !mrs_strcmp(&expected, &actual));

	mrs_setstr(&actual, "123456789", strlen("123456789"));
	mrs_setstr(&append, "10", strlen("10"));
	mrs_setstr(&expected, "12345678910", strlen("12345678910"));

	mrt_group_append_case(t_ctx, "123456789 | 10 over capacity",
			      OK == mrs_strcat(&actual, &append));
	mrt_group_append_case(t_ctx,
			      "123456789 | 10 over capacity length check",
			      11 == actual.capacity);
	mrt_group_append_case(t_ctx, "123456789 | 10 over capacity equals",
			      mrs_strcmp(&expected, &actual));

	mrs_free(&actual);
	mrs_free(&expected);
	mrs_init(9, "123456789", strlen("123456789"), &actual);
	mrs_init(10, "1234567891", strlen("1234567891"), &expected);
	mrs_setstr(&append, "1", strlen("1"));
	mrs_strcat(&actual, &append);

	mrt_group_append_case(t_ctx, "123456789 | 1 exactly capacity",
			      !mrs_strcmp(&expected, &actual));

	mrs_setstr(&actual, "123456789", strlen("123456789"));
	mrs_setstr(&append, "", strlen(""));
	mrs_setstr(&expected, "123456789", strlen("123456789"));
	mrs_strcat(&actual, &append);

	mrt_group_append_case(t_ctx, "123456789 | empty src",
			      !mrs_strcmp(&expected, &actual));

	mrs_free(&actual);
	mrs_free(&expected);
	mrs_init(0, "", strlen(""), &actual);
	mrs_init(0, "123", strlen("123"), &expected);
	mrs_setstr(&append, "123", strlen("123"));

	mrs_strcat(&actual, &append);

	mrt_group_append_case(t_ctx, "123456789 | empty dest",
			      !mrs_strcmp(&expected, &actual));

	mrs_setstr(&actual, "", strlen(""));
	mrs_setstr(&append, "", strlen(""));
	mrs_setstr(&expected, "", strlen(""));
	mrs_strcat(&actual, &append);

	mrt_group_append_case(t_ctx, "empty dest and src",
			      !mrs_strcmp(&expected, &actual));

	mrs_free(&actual);
	mrs_free(&expected);
	mrs_free(&append);
}

void test_pushstr(MrtGroup *t_ctx)
{
	MrsString actual;
	mrs_init(10, "aaabbb", strlen("aaabbb"), &actual);
	const char *append = "ccc";
	MrsString expected;
	mrs_init(10, "aaabbbccc", strlen("aaabbbccc"), &expected);
	mrs_pushstr(&actual, append, strlen(append));

	mrt_group_append_case(t_ctx, "aaabbb | ccc",
			      !mrs_strcmp(&expected, &actual));
	mrt_group_append_case(t_ctx, "aaabbb | ccc capactity",
			      actual.capacity == 10);

	mrs_pushstr(&actual, append, strlen(append));
	mrs_free(&expected);
	mrs_init(12, "aaabbbcccccc", strlen("aaabbbcccccc"), &expected);

	mrt_group_append_case(t_ctx, "aaabbbccc | ccc",
			      !mrs_strcmp(&expected, &actual));
	mrt_group_append_case(t_ctx, "aaabbbccc | ccc capacity",
			      actual.capacity == 12);

	mrs_pushstr(&actual, "", strlen(""));

	mrt_group_append_case(t_ctx, "aaabbbcccccc | ",
			      !mrs_strcmp(&expected, &actual));
	mrt_group_append_case(t_ctx, "aaabbbcccccc | ", actual.capacity == 12);

	mrs_free(&actual);
	mrs_free(&expected);
}

void test_get_char(MrtGroup *t_ctx)
{
	MrsString src;
	mrs_init(10, "0123456789", strlen("0123456789"), &src);

	char actual = mrs_get_char(&src, 3);
	char expected = '3';

	mrt_group_append_case(t_ctx, "within bounds fully",
			      MRT_ASSERT_EQ(expected, actual));

	actual = mrs_get_char(&src, 0);
	expected = '0';

	mrt_group_append_case(t_ctx, "zeroth idx",
			      MRT_ASSERT_EQ(expected, actual));

	actual = mrs_get_char(&src, 9);
	expected = '9';

	mrt_group_append_case(t_ctx, "last idx",
			      MRT_ASSERT_EQ(expected, actual));

	actual = mrs_get_char(&src, 10);
	expected = '\0';

	mrt_group_append_case(t_ctx, "outof bounds",
			      MRT_ASSERT_EQ(expected, actual));

	mrs_free(&src);
}

void test_setstr(MrtGroup *t_ctx)
{
	const char *from = "from";
	const char *to = "to";

	MrsString actual;
	MrsString expected;

	mrs_init(strlen(from), from, strlen(from), &actual);
	int result = mrs_setstr(&actual, to, strlen(to));
	mrs_init(strlen(to), to, strlen(to), &expected);

	mrt_group_append_case(t_ctx, "from -> to",
			      !mrs_strcmp(&expected, &actual));
	mrt_group_append_case(t_ctx, "from -> to result", result == OK);

	result = mrs_setstr(&actual, to, strlen(from) + 1);

	mrt_group_append_case(t_ctx, "from -> from+1 length", result == ERR);

	mrs_free(&actual);
	mrs_free(&expected);
}

void test_setstrn(MrtGroup *t_ctx)
{
	const char *from = "from";
	const char *to = "to";

	MrsString actual;
	MrsString expected;

	mrs_init(strlen(from), from, strlen(from), &actual);
	mrs_init(strlen(to), to, 1, &expected);

	int result = mrs_setstrn(&actual, to, strlen(to), 1);

	mrt_group_append_case(t_ctx, "from -> to length 1",
			      !mrs_strcmp(&expected, &actual));
	mrt_group_append_case(t_ctx, "from -> to length 1 result",
			      result == OK);

	result = mrs_setstrn(&actual, to, strlen(to), 3);

	mrt_group_append_case(t_ctx, "from -> to length 3 result",
			      result == ERR);

	mrs_free(&actual);
	mrs_free(&expected);
}

void test_get_idx(MrtGroup *t_ctx)
{
	const char *from = "from";
	const char *random = "random";

	MrsString example;
	MrsString random_example;

	mrs_init(strlen(random), random, strlen(random), &random_example);
	mrs_init(strlen(from), from, strlen(from), &example);

	uint idx_found;
	int result = mrs_get_idx(&example, &example.value[2], &idx_found);

	mrt_group_append_case(t_ctx, "'from' find from[2]", idx_found == 2);
	mrt_group_append_case(t_ctx, "'from' find from[2] result",
			      result == OK);

	result = mrs_get_idx(&example, &random_example.value[2], &idx_found);

	mrt_group_append_case(t_ctx, "'from' find char* outside result",
			      result == NOT_FOUND);

	mrs_free(&example);
	mrs_free(&random_example);
}

void test_strchr(MrtGroup *t_ctx)
{
	const char *xample_str = "xample_str";
	MrsString xample;
	mrs_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	char *result = mrs_strchr(&xample, 'l');

	mrt_group_append_case(t_ctx, "'xample_str' find 'l'", *result == 'l');

	result = mrs_strchr(&xample, 'r');

	mrt_group_append_case(t_ctx, "'xample_str' find 'r'", *result == 'r');

	result = mrs_strchr(&xample, 'z');

	mrt_group_append_case(t_ctx, "'xample_str' find 'x'",
			      MRT_ASSERT_NULL(result));

	mrs_free(&xample);
}

void test_strndup(MrtGroup *t_ctx)
{
	const char *xample_str = "xample_str";
	MrsString xample;
	MrsString xample_dup;

	mrs_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	int result = mrs_strndup(&xample, xample.len, &xample_dup);

	mrt_group_append_case(t_ctx, "dup full string",
			      !mrs_strcmp(&xample, &xample_dup));
	mrt_group_append_case(t_ctx, "dup full string result", result == OK);

	mrs_free(&xample_dup);

	result = mrs_strndup(&xample, xample.len - 1, &xample_dup);

	mrt_group_append_case(t_ctx, "dup string len - 1",
			      xample_dup.len == xample.len - 1);
	mrt_group_append_case(t_ctx, "dup string len - 1 value check",
			      !strcmp("xample_st", xample_dup.value));
	mrt_group_append_case(t_ctx, "dup string len - 1 result", result == OK);

	mrs_free(&xample_dup);

	result = mrs_strndup(&xample, xample.len + 1, &xample_dup);

	mrt_group_append_case(t_ctx, "dup string len + 1", result == ERR);

	mrs_free(&xample_dup);
	mrs_free(&xample);
}

void test_shrink_to_fit(MrtGroup *t_ctx)
{
	const char *example_str = "example_str";
	MrsString example;
	mrs_init(32, example_str, strlen(example_str), &example);

	mrs_shrink_to_fit(&example);

	mrt_group_append_case(t_ctx, "example_str capacity 32 length check",
			      MRT_ASSERT_EQ(example.capacity, example.len));
	mrt_group_append_case(t_ctx, "example_str capacity 32 value check",
			      strcmp(example.value, example_str) == 0);

	mrs_free(&example);
}

void test_trim_trailing_whitespace(MrtGroup *t_ctx)
{
	MrsString test;
	mrs_init(20, "1234567890", strlen("1234567890"), &test);
	mrs_trim_trailing_whitespace(&test);

	mrt_group_append_case(t_ctx, "trim no trailing whitespace",
			      test.len == 10);

	mrs_free(&test);

	mrs_init(20, "123    890", strlen("123    890"), &test);
	mrs_trim_trailing_whitespace(&test);

	mrt_group_append_case(t_ctx,
			      "trim no trailing whitespace. inner whitespace",
			      test.len == 10);

	mrs_free(&test);

	mrs_init(20, "        ", strlen("        "), &test);
	mrs_trim_trailing_whitespace(&test);

	mrt_group_append_case(t_ctx, "only whitespace", test.len == 0);

	mrs_free(&test);

	mrs_init(20, "qqqqq   ", strlen("qqqqq   "), &test);
	mrs_trim_trailing_whitespace(&test);

	mrt_group_append_case(t_ctx, "trailing whitespace", test.len == 5);

	mrs_free(&test);

	mrs_init(20, "q   q   ", strlen("q   q   "), &test);
	mrs_trim_trailing_whitespace(&test);

	mrt_group_append_case(t_ctx,
			      "trailing whitespace white inner whitespace",
			      test.len == 5);

	mrs_free(&test);
}

void test_append(MrtGroup *t_ctx)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));
	int append_val = CAFE_BABE;
	mrv_append(int_array, &append_val);

	mrt_group_append_case(t_ctx, "empty array append within capacity len",
			      int_array->len == 1);
	mrt_group_append_case(t_ctx,
			      "empty array append within capacity capacity",
			      int_array->capacity == 10);
	mrt_group_append_case(t_ctx,
			      "empty array append within capacity stride",
			      int_array->stride == sizeof(int));

	int *val = mrv_get_idx(int_array, 0);

	mrt_group_append_case(t_ctx, "empty array append within capacity val",
			      *val == append_val);

	mrv_destroy(int_array);

	int_array = mrv_create(10, sizeof(int));

	mrt_group_append_case(t_ctx, "empty array append to capacity capacity",
			      int_array->capacity == 10);

	for (size_t i = 0; i < 10; i++) {
		mrv_append(int_array, &i);
		val = mrv_get_idx(int_array, i);

		mrt_group_append_case(t_ctx,
				      "empty array append to capacity value",
				      *val == (int)i);
	}

	mrt_group_append_case(t_ctx, "empty array append to capacity len",
			      int_array->len == 10);

	Err error = mrv_append(int_array, &append_val);
	val = mrv_get_idx(int_array, 10);

	mrt_group_append_case(t_ctx, "empty array append over capacity OK",
			      error == OK);
	mrt_group_append_case(t_ctx, "empty array append over capacity value",
			      *val == append_val);
	mrt_group_append_case(t_ctx, "empty array append over capacity len",
			      int_array->len == 11);
	mrt_group_append_case(t_ctx,
			      "empty array append over capacity capacity",
			      int_array->capacity == 11);

	mrv_destroy(int_array);
}

void test_pop(MrtGroup *t_ctx)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));
	int append_val = CAFE_BABE;
	mrv_append(int_array, &append_val);
	mrv_append(int_array, &append_val);
	mrv_append(int_array, &append_val);
	mrv_pop(int_array);

	mrt_group_append_case(t_ctx, "pop len > 0 len", int_array->len == 2);
	mrt_group_append_case(t_ctx, "pop len > 0 capacity",
			      int_array->capacity == 10);

	mrv_pop(int_array);
	Err error = mrv_pop(int_array);

	mrt_group_append_case(t_ctx, "pop to len = 0 len", int_array->len == 0);
	mrt_group_append_case(t_ctx, "pop to len = 0 capacity",
			      int_array->capacity == 10);
	mrt_group_append_case(t_ctx, "pop to len = 0 err", error == OK);

	error = mrv_pop(int_array);

	mrt_group_append_case(t_ctx, "pop at len = 0 len", int_array->len == 0);
	mrt_group_append_case(t_ctx, "pop at len = 0 err", error == ERR);

	mrv_destroy(int_array);
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

void test_get_item_where(MrtGroup *t_ctx)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));
	int append_val = 1;
	mrv_append(int_array, &append_val);
	append_val++;
	mrv_append(int_array, &append_val);
	append_val++;
	mrv_append(int_array, &append_val);
	append_val++;

	void *two_ptr = mrv_get_idx(int_array, 1);

	mrt_group_append_case(t_ctx, "get middle item",
			      mrv_get_item_where(int_array, is_two) == two_ptr);
	mrt_group_append_case(t_ctx, "item doesnt exist",
			      mrv_get_item_where(int_array, always_false) ==
				      NULL);

	mrv_destroy(int_array);
}

void test_get_item(MrtGroup *t_ctx)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));
	int append_val = 1;
	mrv_append(int_array, &append_val);
	append_val++;
	mrv_append(int_array, &append_val);
	append_val++;
	mrv_append(int_array, &append_val);
	append_val++;

	void *two_ptr = mrv_get_idx(int_array, 1);

	append_val = 2;

	mrt_group_append_case(t_ctx, "get middle item",
			      mrv_get_item(int_array, &append_val) == two_ptr);

	append_val = CAFE_BABE;

	mrt_group_append_case(t_ctx, "item doesnt exist",
			      mrv_get_item(int_array, &append_val) == NULL);

	mrv_destroy(int_array);
}

int main(void)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrtCtx *ctx = mrt_ctx_create(logger);

	// mrs_strings
	mrt_ctx_register_test_func(ctx, test_strstr, "test_strstr");
	mrt_ctx_register_test_func(ctx, test_filter, "test_filter");
	mrt_ctx_register_test_func(ctx, test_strcat, "test_strcat");
	mrt_ctx_register_test_func(ctx, test_pushstr, "test_pushstr");
	mrt_ctx_register_test_func(ctx, test_get_char, "test_get_char");
	mrt_ctx_register_test_func(ctx, test_setstr, "test_setstr");
	mrt_ctx_register_test_func(ctx, test_setstrn, "test_setstrn");
	mrt_ctx_register_test_func(ctx, test_get_idx, "test_get_idx");
	mrt_ctx_register_test_func(ctx, test_strchr, "test_strchr");
	mrt_ctx_register_test_func(ctx, test_strndup, "test_strndup");
	mrt_ctx_register_test_func(ctx, test_shrink_to_fit,
				   "test_shrink_to_fit");
	mrt_ctx_register_test_func(ctx, test_trim_trailing_whitespace,
				   "test_trim_trailing_whitespace");

	// mrv_vectors
	mrt_ctx_register_test_func(ctx, test_append, "test_append");
	mrt_ctx_register_test_func(ctx, test_pop, "test_pop");
	mrt_ctx_register_test_func(ctx, test_get_item_where,
				   "test_get_item_where");
	mrt_ctx_register_test_func(ctx, test_get_item, "test_get_item");

	Err err = mrt_ctx_run(ctx);

	mrt_ctx_destroy(ctx);
	mrl_destroy(logger);

	return err;
}
