#include <stdlib.h>
#include <string.h>

#include <mr_utils.h>
#include <mr_utils/mrt_test.h>

void test_strstr(MrtGroup *t_ctx)
{
	const char *haystack = "11151111111111111231";
	const char *needle = "31";

	MrsString a;
	mrs_init(0, haystack, strlen(haystack), &a);

	MrsString b;
	mrs_init(3, needle, strlen(needle), &b);

	MRT_ASSERT(t_ctx, a.value[18] == *mrs_strstr(&a, &b, 0),
		   "11151111111111111231 | 31");

	size_t start_idx = 3;

	MRT_ASSERT(t_ctx, a.value[18] == *mrs_strstr(&a, &b, start_idx),
		   "11151111111111111231 | 31 startidx=3");

	MRT_ASSERT(t_ctx, a.value[18] == *mrs_strstr(&a, &b, 0),
		   "11151111111111111231 | 31 startidx=NULL");

	mrs_setstr(&b, "52", strlen("52"));

	MRT_ASSERT(t_ctx, mrs_strstr(&a, &b, 0) == NULL,
		   "11151111111111111231 | 52");

	mrs_setstr(&b, "112", strlen("112"));

	MRT_ASSERT(t_ctx, a.value[15] == *mrs_strstr(&a, &b, 0),
		   "11151111111111111231 | 112");

	mrs_setstr(&b, "311", strlen("311"));

	MRT_ASSERT(t_ctx, mrs_strstr(&a, &b, 0) == NULL,
		   "11151111111111111231 | 311 OOB");

	mrs_free(&a);
	mrs_free(&b);
	return;
}

static void test_filter(MrtGroup *t_ctx)
{
	MrsString actual;
	mrs_init(20, "12121", strlen("12121"), &actual);
	MrsString expected;
	mrs_init(20, "111", strlen("111"), &expected);
	mrs_filter(&actual, '2');

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual), "12121 | 2");

	mrs_setstr(&actual, "2222", strlen("2222"));
	mrs_setstr(&expected, "", strlen(""));
	mrs_filter(&actual, '2');

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual), "2222 | 2");

	mrs_setstr(&actual, "2222", strlen("2222"));
	mrs_setstr(&expected, "2222", strlen("2222"));
	mrs_filter(&actual, '3');

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual), "2222 | 3");

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

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual), "aaabbb | cc");

	mrs_setstr(&actual, "123456789", strlen("123456789"));
	mrs_setstr(&append, "10", strlen("10"));
	mrs_setstr(&expected, "12345678910", strlen("12345678910"));

	MRT_ASSERT(t_ctx, OK == mrs_strcat(&actual, &append),
		   "123456789 | 10 over capacity");
	MRT_ASSERT(t_ctx, 11 == actual.capacity,
		   "123456789 | 10 over capacity length check");
	MRT_ASSERT(t_ctx, mrs_strcmp(&expected, &actual),
		   "123456789 | 10 over capacity equals");

	mrs_free(&actual);
	mrs_free(&expected);
	mrs_init(9, "123456789", strlen("123456789"), &actual);
	mrs_init(10, "1234567891", strlen("1234567891"), &expected);
	mrs_setstr(&append, "1", strlen("1"));
	mrs_strcat(&actual, &append);

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual),
		   "123456789 | 1 exactly capacity");

	mrs_setstr(&actual, "123456789", strlen("123456789"));
	mrs_setstr(&append, "", strlen(""));
	mrs_setstr(&expected, "123456789", strlen("123456789"));
	mrs_strcat(&actual, &append);

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual),
		   "123456789 | empty src");

	mrs_free(&actual);
	mrs_free(&expected);
	mrs_init(0, "", strlen(""), &actual);
	mrs_init(0, "123", strlen("123"), &expected);
	mrs_setstr(&append, "123", strlen("123"));

	mrs_strcat(&actual, &append);

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual),
		   "123456789 | empty dest");

	mrs_setstr(&actual, "", strlen(""));
	mrs_setstr(&append, "", strlen(""));
	mrs_setstr(&expected, "", strlen(""));
	mrs_strcat(&actual, &append);

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual),
		   "empty dest and src");

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

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual), "aaabbb | ccc");
	MRT_ASSERT(t_ctx, actual.capacity == 10, "aaabbb | ccc capactity");

	mrs_pushstr(&actual, append, strlen(append));
	mrs_free(&expected);
	mrs_init(12, "aaabbbcccccc", strlen("aaabbbcccccc"), &expected);

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual), "aaabbbccc | ccc");
	MRT_ASSERT(t_ctx, actual.capacity == 12, "aaabbbccc | ccc capacity");

	mrs_pushstr(&actual, "", strlen(""));

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual), "aaabbbcccccc | ");
	MRT_ASSERT(t_ctx, actual.capacity == 12, "aaabbbcccccc | ");

	mrs_free(&actual);
	mrs_free(&expected);
}

void test_get_char(MrtGroup *t_ctx)
{
	MrsString src;
	mrs_init(10, "0123456789", strlen("0123456789"), &src);

	char actual = mrs_get_char(&src, 3);
	char expected = '3';

	MRT_ASSERT(t_ctx, expected == actual, "within bounds fully");

	actual = mrs_get_char(&src, 0);
	expected = '0';

	MRT_ASSERT(t_ctx, expected == actual, "zeroth idx");

	actual = mrs_get_char(&src, 9);
	expected = '9';

	MRT_ASSERT(t_ctx, expected == actual, "last idx");

	actual = mrs_get_char(&src, 10);
	expected = '\0';

	MRT_ASSERT(t_ctx, expected == actual, "outof bounds");

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

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual), "from -> to");
	MRT_ASSERT(t_ctx, result == OK, "from -> to result");

	result = mrs_setstr(&actual, to, strlen(from) + 1);

	MRT_ASSERT(t_ctx, result == ERR, "from -> from+1 length");

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

	MRT_ASSERT(t_ctx, !mrs_strcmp(&expected, &actual),
		   "from -> to length 1");
	MRT_ASSERT(t_ctx, result == OK, "from -> to length 1 result");

	result = mrs_setstrn(&actual, to, strlen(to), 3);

	MRT_ASSERT(t_ctx, result == ERR, "from -> to length 3 result");

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

	MRT_ASSERT(t_ctx, idx_found == 2, "'from' find from[2]");

	MRT_ASSERT(t_ctx, result == OK, "'from' find from[2] result");

	result = mrs_get_idx(&example, &random_example.value[2], &idx_found);

	MRT_ASSERT(t_ctx, result == NOT_FOUND,
		   "'from' find char* outside result");

	mrs_free(&example);
	mrs_free(&random_example);
}

void test_strchr(MrtGroup *t_ctx)
{
	const char *xample_str = "xample_str";
	MrsString xample;
	mrs_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	char *result = mrs_strchr(&xample, 'l');

	MRT_ASSERT(t_ctx, *result == 'l', "'xample_str' find 'l'");

	result = mrs_strchr(&xample, 'r');

	MRT_ASSERT(t_ctx, *result == 'r', "'xample_str' find 'r'");

	result = mrs_strchr(&xample, 'z');

	MRT_ASSERT(t_ctx, result == NULL, "'xample_str' find 'x'");

	mrs_free(&xample);
}

void test_strndup(MrtGroup *t_ctx)
{
	const char *xample_str = "xample_str";
	MrsString xample;
	MrsString xample_dup;

	mrs_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	int result = mrs_strndup(&xample, xample.len, &xample_dup);

	MRT_ASSERT(t_ctx, !mrs_strcmp(&xample, &xample_dup), "dup full string");
	MRT_ASSERT(t_ctx, result == OK, "dup full string result");

	mrs_free(&xample_dup);

	result = mrs_strndup(&xample, xample.len - 1, &xample_dup);

	MRT_ASSERT(t_ctx, xample_dup.len == xample.len - 1,
		   "dup string len - 1");
	MRT_ASSERT(t_ctx, !strcmp("xample_st", xample_dup.value),
		   "dup string len - 1 value check");
	MRT_ASSERT(t_ctx, result == OK, "dup string len - 1 result");

	mrs_free(&xample_dup);

	result = mrs_strndup(&xample, xample.len + 1, &xample_dup);

	MRT_ASSERT(t_ctx, result == ERR, "dup string len + 1");

	mrs_free(&xample_dup);
	mrs_free(&xample);
}

void test_shrink_to_fit(MrtGroup *t_ctx)
{
	const char *example_str = "example_str";
	MrsString example;
	mrs_init(32, example_str, strlen(example_str), &example);

	mrs_shrink_to_fit(&example);

	MRT_ASSERT(t_ctx, example.capacity == example.len,
		   "example_str capacity 32 length check");
	MRT_ASSERT(t_ctx, strcmp(example.value, example_str) == 0,
		   "example_str capacity 32 value check");

	mrs_free(&example);
}

void test_trim_trailing_whitespace(MrtGroup *t_ctx)
{
	MrsString test;
	mrs_init(20, "1234567890", strlen("1234567890"), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(t_ctx, test.len == 10, "trim no trailing whitespace");

	mrs_free(&test);

	mrs_init(20, "123    890", strlen("123    890"), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(t_ctx, test.len == 10,
		   "trim no trailing whitespace. inner whitespace");

	mrs_free(&test);

	mrs_init(20, "        ", strlen("        "), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(t_ctx, test.len == 0, "only whitespace");

	mrs_free(&test);

	mrs_init(20, "qqqqq   ", strlen("qqqqq   "), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(t_ctx, test.len == 5, "trailing whitespace");

	mrs_free(&test);

	mrs_init(20, "q   q   ", strlen("q   q   "), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(t_ctx, test.len == 5,
		   "trailing whitespace white inner whitespace");

	mrs_free(&test);
}

void test_append(MrtGroup *t_ctx)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));
	int append_val = CAFE_BABE;
	mrv_append(int_array, &append_val);

	MRT_ASSERT(t_ctx, int_array->len == 1,
		   "empty array append within capacity len");
	MRT_ASSERT(t_ctx, int_array->capacity == 10,
		   "empty array append within capacity capacity");
	MRT_ASSERT(t_ctx, int_array->stride == sizeof(int),
		   "empty array append within capacity stride");

	int *val = mrv_get_idx(int_array, 0);

	MRT_ASSERT(t_ctx, *val == append_val,
		   "empty array append within capacity val");

	mrv_destroy(int_array);

	int_array = mrv_create(10, sizeof(int));

	MRT_ASSERT(t_ctx, int_array->capacity == 10,
		   "empty array append to capacity capacity");

	for (size_t i = 0; i < 10; i++) {
		mrv_append(int_array, &i);
		val = mrv_get_idx(int_array, i);

		MRT_ASSERT(t_ctx, *val == (int)i,
			   "empty array append to capacity value");
	}

	MRT_ASSERT(t_ctx, int_array->len == 10,
		   "empty array append to capacity len");

	Err error = mrv_append(int_array, &append_val);
	val = mrv_get_idx(int_array, 10);

	MRT_ASSERT(t_ctx, error == OK, "empty array append over capacity OK");
	MRT_ASSERT(t_ctx, *val == append_val,
		   "empty array append over capacity value");
	MRT_ASSERT(t_ctx, int_array->len == 11,
		   "empty array append over capacity len");
	MRT_ASSERT(t_ctx, int_array->capacity == 11,
		   "empty array append over capacity capacity");

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

	MRT_ASSERT(t_ctx, int_array->len == 2, "pop len > 0 len");
	MRT_ASSERT(t_ctx, int_array->capacity == 10, "pop len > 0 capacity");

	mrv_pop(int_array);
	Err error = mrv_pop(int_array);

	MRT_ASSERT(t_ctx, int_array->len == 0, "pop to len = 0 len");
	MRT_ASSERT(t_ctx, int_array->capacity == 10, "pop to len = 0 capacity");
	MRT_ASSERT(t_ctx, error == OK, "pop to len = 0 err");

	error = mrv_pop(int_array);

	MRT_ASSERT(t_ctx, int_array->len == 0, "pop at len = 0 len");
	MRT_ASSERT(t_ctx, error == ERR, "pop at len = 0 err");

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

	MRT_ASSERT(t_ctx, mrv_get_item_where(int_array, is_two) == two_ptr,
		   "get middle item");
	MRT_ASSERT(t_ctx, mrv_get_item_where(int_array, always_false) == NULL,
		   "item doesnt exist");

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

	MRT_ASSERT(t_ctx, mrv_get_item(int_array, &append_val) == two_ptr,
		   "get middle item");

	append_val = CAFE_BABE;

	MRT_ASSERT(t_ctx, mrv_get_item(int_array, &append_val) == NULL,
		   "item doesnt exist");

	mrv_destroy(int_array);
}

int main(void)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrtContext *ctx = mrt_ctx_create(logger);

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
