#include <stdlib.h>
#include <string.h>

#include <mr_utils.h>
#include <mr_utils/mrt_test.h>

MRT_TEST_GROUP(test_strstr)
{
	const char *haystack = "11151111111111111231";
	const char *needle = "31";

	MrsString a;
	mrs_init(0, haystack, strlen(haystack), &a);

	MrsString b;
	mrs_init(3, needle, strlen(needle), &b);

	MRT_ASSERT(a.value[18] == *mrs_strstr(&a, &b, 0),
		   "11151111111111111231 | 31");

	size_t start_idx = 3;

	MRT_ASSERT(a.value[18] == *mrs_strstr(&a, &b, start_idx),
		   "11151111111111111231 | 31 startidx=3");

	MRT_ASSERT(a.value[18] == *mrs_strstr(&a, &b, 0),
		   "11151111111111111231 | 31 startidx=NULL");

	mrs_setstr(&b, "52", strlen("52"));

	MRT_ASSERT(mrs_strstr(&a, &b, 0) == NULL, "11151111111111111231 | 52");

	mrs_setstr(&b, "112", strlen("112"));

	MRT_ASSERT(a.value[15] == *mrs_strstr(&a, &b, 0),
		   "11151111111111111231 | 112");

	mrs_setstr(&b, "311", strlen("311"));

	MRT_ASSERT(mrs_strstr(&a, &b, 0) == NULL,
		   "11151111111111111231 | 311 OOB");

	mrs_free(&a);
	mrs_free(&b);
	return;
}

MRT_TEST_GROUP(test_filter)
{
	MrsString actual;
	mrs_init(20, "12121", strlen("12121"), &actual);
	MrsString expected;
	mrs_init(20, "111", strlen("111"), &expected);
	mrs_filter(&actual, '2');

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "12121 | 2");

	mrs_setstr(&actual, "2222", strlen("2222"));
	mrs_setstr(&expected, "", strlen(""));
	mrs_filter(&actual, '2');

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "2222 | 2");

	mrs_setstr(&actual, "2222", strlen("2222"));
	mrs_setstr(&expected, "2222", strlen("2222"));
	mrs_filter(&actual, '3');

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "2222 | 3");

	mrs_free(&actual);
	mrs_free(&expected);
}

MRT_TEST_GROUP(test_strcat)
{
	MrsString actual;
	mrs_init(10, "aaabbb", strlen("aaabbb"), &actual);
	MrsString append;
	mrs_init(10, "cc", strlen("cc"), &append);
	MrsString expected;
	mrs_init(10, "aaabbbcc", strlen("aaabbbcc"), &expected);
	mrs_strcat(&actual, &append);

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "aaabbb | cc");

	mrs_setstr(&actual, "123456789", strlen("123456789"));
	mrs_setstr(&append, "10", strlen("10"));
	mrs_setstr(&expected, "12345678910", strlen("12345678910"));

	MRT_ASSERT(OK == mrs_strcat(&actual, &append),
		   "123456789 | 10 over capacity");
	MRT_ASSERT(11 == actual.capacity,
		   "123456789 | 10 over capacity length check");
	MRT_ASSERT(mrs_strcmp(&expected, &actual),
		   "123456789 | 10 over capacity equals");

	mrs_free(&actual);
	mrs_free(&expected);
	mrs_init(9, "123456789", strlen("123456789"), &actual);
	mrs_init(10, "1234567891", strlen("1234567891"), &expected);
	mrs_setstr(&append, "1", strlen("1"));
	mrs_strcat(&actual, &append);

	MRT_ASSERT(!mrs_strcmp(&expected, &actual),
		   "123456789 | 1 exactly capacity");

	mrs_setstr(&actual, "123456789", strlen("123456789"));
	mrs_setstr(&append, "", strlen(""));
	mrs_setstr(&expected, "123456789", strlen("123456789"));
	mrs_strcat(&actual, &append);

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "123456789 | empty src");

	mrs_free(&actual);
	mrs_free(&expected);
	mrs_init(0, "", strlen(""), &actual);
	mrs_init(0, "123", strlen("123"), &expected);
	mrs_setstr(&append, "123", strlen("123"));

	mrs_strcat(&actual, &append);

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "123456789 | empty dest");

	mrs_setstr(&actual, "", strlen(""));
	mrs_setstr(&append, "", strlen(""));
	mrs_setstr(&expected, "", strlen(""));
	mrs_strcat(&actual, &append);

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "empty dest and src");

	mrs_free(&actual);
	mrs_free(&expected);
	mrs_free(&append);
}

MRT_TEST_GROUP(test_pushstr)
{
	MrsString actual;
	mrs_init(10, "aaabbb", strlen("aaabbb"), &actual);
	const char *append = "ccc";
	MrsString expected;
	mrs_init(10, "aaabbbccc", strlen("aaabbbccc"), &expected);
	mrs_pushstr(&actual, append, strlen(append));

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "aaabbb | ccc");
	MRT_ASSERT(actual.capacity == 10, "aaabbb | ccc capactity");

	mrs_pushstr(&actual, append, strlen(append));
	mrs_free(&expected);
	mrs_init(12, "aaabbbcccccc", strlen("aaabbbcccccc"), &expected);

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "aaabbbccc | ccc");
	MRT_ASSERT(actual.capacity == 12, "aaabbbccc | ccc capacity");

	mrs_pushstr(&actual, "", strlen(""));

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "aaabbbcccccc | ");
	MRT_ASSERT(actual.capacity == 12, "aaabbbcccccc | ");

	mrs_free(&actual);
	mrs_free(&expected);
}

MRT_TEST_GROUP(test_get_char)
{
	MrsString src;
	mrs_init(10, "0123456789", strlen("0123456789"), &src);

	char actual = mrs_get_char(&src, 3);
	char expected = '3';

	MRT_ASSERT(expected == actual, "within bounds fully");

	actual = mrs_get_char(&src, 0);
	expected = '0';

	MRT_ASSERT(expected == actual, "zeroth idx");

	actual = mrs_get_char(&src, 9);
	expected = '9';

	MRT_ASSERT(expected == actual, "last idx");

	actual = mrs_get_char(&src, 10);
	expected = '\0';

	MRT_ASSERT(expected == actual, "outof bounds");

	mrs_free(&src);
}

MRT_TEST_GROUP(test_setstr)
{
	const char *from = "from";
	const char *to = "to";

	MrsString actual;
	MrsString expected;

	mrs_init(strlen(from), from, strlen(from), &actual);
	int result = mrs_setstr(&actual, to, strlen(to));
	mrs_init(strlen(to), to, strlen(to), &expected);

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "from -> to");
	MRT_ASSERT(result == OK, "from -> to result");

	result = mrs_setstr(&actual, to, strlen(from) + 1);

	MRT_ASSERT(result == ERR, "from -> from+1 length");

	mrs_free(&actual);
	mrs_free(&expected);
}

MRT_TEST_GROUP(test_setstrn)
{
	const char *from = "from";
	const char *to = "to";

	MrsString actual;
	MrsString expected;

	mrs_init(strlen(from), from, strlen(from), &actual);
	mrs_init(strlen(to), to, 1, &expected);

	int result = mrs_setstrn(&actual, to, strlen(to), 1);

	MRT_ASSERT(!mrs_strcmp(&expected, &actual), "from -> to length 1");
	MRT_ASSERT(result == OK, "from -> to length 1 result");

	result = mrs_setstrn(&actual, to, strlen(to), 3);

	MRT_ASSERT(result == ERR, "from -> to length 3 result");

	mrs_free(&actual);
	mrs_free(&expected);
}

MRT_TEST_GROUP(test_get_idx)
{
	const char *from = "from";
	const char *random = "random";

	MrsString example;
	MrsString random_example;

	mrs_init(strlen(random), random, strlen(random), &random_example);
	mrs_init(strlen(from), from, strlen(from), &example);

	uint idx_found;
	int result = mrs_get_idx(&example, &example.value[2], &idx_found);

	MRT_ASSERT(idx_found == 2, "'from' find from[2]");

	MRT_ASSERT(result == OK, "'from' find from[2] result");

	result = mrs_get_idx(&example, &random_example.value[2], &idx_found);

	MRT_ASSERT(result == NOT_FOUND, "'from' find char* outside result");

	mrs_free(&example);
	mrs_free(&random_example);
}

MRT_TEST_GROUP(test_strchr)
{
	const char *xample_str = "xample_str";
	MrsString xample;
	mrs_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	char *result = mrs_strchr(&xample, 'l');

	MRT_ASSERT(*result == 'l', "'xample_str' find 'l'");

	result = mrs_strchr(&xample, 'r');

	MRT_ASSERT(*result == 'r', "'xample_str' find 'r'");

	result = mrs_strchr(&xample, 'z');

	MRT_ASSERT(result == NULL, "'xample_str' find 'x'");

	mrs_free(&xample);
}

MRT_TEST_GROUP(test_strndup)
{
	const char *xample_str = "xample_str";
	MrsString xample;
	MrsString xample_dup;

	mrs_init(strlen(xample_str), xample_str, strlen(xample_str), &xample);

	int result = mrs_strndup(&xample, xample.len, &xample_dup);

	MRT_ASSERT(!mrs_strcmp(&xample, &xample_dup), "dup full string");
	MRT_ASSERT(result == OK, "dup full string result");

	mrs_free(&xample_dup);

	result = mrs_strndup(&xample, xample.len - 1, &xample_dup);

	MRT_ASSERT(xample_dup.len == xample.len - 1, "dup string len - 1");
	MRT_ASSERT(!strcmp("xample_st", xample_dup.value),
		   "dup string len - 1 value check");
	MRT_ASSERT(result == OK, "dup string len - 1 result");

	mrs_free(&xample_dup);

	result = mrs_strndup(&xample, xample.len + 1, &xample_dup);

	MRT_ASSERT(result == ERR, "dup string len + 1");

	mrs_free(&xample_dup);
	mrs_free(&xample);
}

MRT_TEST_GROUP(test_shrink_to_fit)
{
	const char *example_str = "example_str";
	MrsString example;
	mrs_init(32, example_str, strlen(example_str), &example);

	mrs_shrink_to_fit(&example);

	MRT_ASSERT(example.capacity == example.len,
		   "example_str capacity 32 length check");
	MRT_ASSERT(strcmp(example.value, example_str) == 0,
		   "example_str capacity 32 value check");

	mrs_free(&example);
}

MRT_TEST_GROUP(test_trim_trailing_whitespace)
{
	MrsString test;
	mrs_init(20, "1234567890", strlen("1234567890"), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(test.len == 10, "trim no trailing whitespace");

	mrs_free(&test);

	mrs_init(20, "123    890", strlen("123    890"), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(test.len == 10,
		   "trim no trailing whitespace. inner whitespace");

	mrs_free(&test);

	mrs_init(20, "        ", strlen("        "), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(test.len == 0, "only whitespace");

	mrs_free(&test);

	mrs_init(20, "qqqqq   ", strlen("qqqqq   "), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(test.len == 5, "trailing whitespace");

	mrs_free(&test);

	mrs_init(20, "q   q   ", strlen("q   q   "), &test);
	mrs_trim_trailing_whitespace(&test);

	MRT_ASSERT(test.len == 5, "trailing whitespace white inner whitespace");

	mrs_free(&test);
}

MRT_TEST_GROUP(test_append)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));
	int append_val = CAFE_BABE;
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);

	MRT_ASSERT(int_array->len == 1,
		   "empty array append within capacity len");
	MRT_ASSERT(int_array->capacity == 10,
		   "empty array append within capacity capacity");
	MRT_ASSERT(int_array->stride == sizeof(int),
		   "empty array append within capacity stride");

	int *val = mrv_get_idx(int_array, 0);

	MRT_ASSERT(*val == append_val,
		   "empty array append within capacity val");

	mrv_destroy(int_array);

	int_array = mrv_create(10, sizeof(int));

	MRT_ASSERT(int_array->capacity == 10,
		   "empty array append to capacity capacity");

	for (size_t i = 0; i < 10; i++) {
		mrv_append(int_array, &i, APPEND_SCALING_INCREMENT);
		val = mrv_get_idx(int_array, i);

		MRT_ASSERT(*val == (int)i,
			   "empty array append to capacity value");
	}

	MRT_ASSERT(int_array->len == 10, "empty array append to capacity len");

	Err error =
		mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	val = mrv_get_idx(int_array, 10);

	MRT_ASSERT(error == OK, "empty array append over capacity OK");
	MRT_ASSERT(*val == append_val,
		   "empty array append over capacity value");
	MRT_ASSERT(int_array->len == 11,
		   "empty array append over capacity len");
	MRT_ASSERT(int_array->capacity == 11,
		   "empty array append over capacity capacity");

	error = mrv_append(int_array, &append_val, APPEND_SCALING_DOUBLE);
	MRT_ASSERT(error == OK, "append with double capacity OK");
	MRT_ASSERT(int_array->capacity == 22,
		   "append with double capacity capacity");

	mrv_realloc_to_fit(int_array); // array now capacity = 12

	error = mrv_append(int_array, &append_val,
			   APPEND_SCALING_ONE_POINT_FIVE);
	MRT_ASSERT(error == OK, "append with 1.5 capacity OK");
	MRT_ASSERT(int_array->capacity == 18,
		   "append with 1.5 capacity capacity");

	mrv_destroy(int_array);
}

MRT_TEST_GROUP(test_pop)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));
	int append_val = CAFE_BABE;
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	mrv_pop(int_array);

	MRT_ASSERT(int_array->len == 2, "pop len > 0 len");
	MRT_ASSERT(int_array->capacity == 10, "pop len > 0 capacity");

	mrv_pop(int_array);
	Err error = mrv_pop(int_array);

	MRT_ASSERT(int_array->len == 0, "pop to len = 0 len");
	MRT_ASSERT(int_array->capacity == 10, "pop to len = 0 capacity");
	MRT_ASSERT(error == OK, "pop to len = 0 err");

	error = mrv_pop(int_array);

	MRT_ASSERT(int_array->len == 0, "pop at len = 0 len");
	MRT_ASSERT(error == ERR, "pop at len = 0 err");

	mrv_destroy(int_array);
}

MRT_TEST_GROUP(test_remove)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));

	for (int i = 0; i < 5; i++) {
		mrv_append(int_array, &i, APPEND_SCALING_INCREMENT);
	}

	MRT_ASSERT(int_array->len == 5, "setup length correct");

	Err res = mrv_remove(int_array, 2);
	MRT_ASSERT(res == OK, "remove middle OK");
	MRT_ASSERT(int_array->len == 4, "remove middle len update");

	int *val = mrv_get_idx(int_array, 2);
	MRT_ASSERT(val && *val == 4,
		   "unordered remove: last element moved to middle");

	res = mrv_remove(int_array, 0);
	MRT_ASSERT(res == OK, "remove head OK");
	MRT_ASSERT(int_array->len == 3, "remove head len update");

	val = mrv_get_idx(int_array, 0);
	MRT_ASSERT(val && *val == 3,
		   "unordered remove: last element moved to head");

	res = mrv_remove(int_array, 2);
	MRT_ASSERT(res == OK, "remove tail OK");
	MRT_ASSERT(int_array->len == 2, "remove tail len update");

	val = mrv_get_idx(int_array, 2);
	MRT_ASSERT(val == NULL, "remove tail: old index inaccessible");

	val = mrv_get_idx(int_array, 0);
	MRT_ASSERT(val && *val == 3, "integrity check: index 0 still holds 3");

	res = mrv_remove(int_array, 100);
	MRT_ASSERT(res == ERR, "remove invalid index returns ERR");
	MRT_ASSERT(int_array->len == 2, "remove invalid index len unchanged");

	mrv_remove(int_array, 0);
	res = mrv_remove(int_array, 0);

	MRT_ASSERT(res == OK, "remove last item OK");
	MRT_ASSERT(int_array->len == 0, "remove until empty len is 0");

	res = mrv_remove(int_array, 0);
	MRT_ASSERT(res == ERR, "remove from empty returns ERR");

	mrv_destroy(int_array);
}

MRT_TEST_GROUP(test_get_pos)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));

	for (int i = 0; i < 10; i++) {
		mrv_append(int_array, &i, APPEND_SCALING_INCREMENT);
	}

	void *first_item = mrv_get_idx(int_array, 0);
	size_t pos_first = mrv_get_pos(int_array, first_item);
	MRT_ASSERT(pos_first == 0, "get_pos for first element");

	void *middle_item = mrv_get_idx(int_array, 5);
	size_t pos_middle = mrv_get_pos(int_array, middle_item);
	MRT_ASSERT(pos_middle == 5, "get_pos for middle element");

	void *last_item = mrv_get_idx(int_array, 9);
	size_t pos_last = mrv_get_pos(int_array, last_item);
	MRT_ASSERT(pos_last == 9, "get_pos for last element");

	mrv_destroy(int_array);

	typedef struct {
		long a;
		long b;
	} LargeData;

	MrvVector *large_array = mrv_create(5, sizeof(LargeData));
	LargeData data = { 100, 200 };

	for (int i = 0; i < 5; i++) {
		mrv_append(large_array, &data, APPEND_SCALING_INCREMENT);
	}

	void *large_item = mrv_get_idx(large_array, 3);
	size_t pos_large = mrv_get_pos(large_array, large_item);

	MRT_ASSERT(large_array->stride == sizeof(LargeData),
		   "large array stride check");
	MRT_ASSERT(pos_large == 3, "get_pos for large data type index 3");

	mrv_destroy(large_array);
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

MRT_TEST_GROUP(test_get_item_where)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));
	int append_val = 1;
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	append_val++;
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	append_val++;
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	append_val++;

	void *two_ptr = mrv_get_idx(int_array, 1);

	MRT_ASSERT(mrv_get_item_where(int_array, is_two) == two_ptr,
		   "get middle item");
	MRT_ASSERT(mrv_get_item_where(int_array, always_false) == NULL,
		   "item doesnt exist");

	mrv_destroy(int_array);
}

MRT_TEST_GROUP(test_get_item)
{
	MrvVector *int_array = mrv_create(10, sizeof(int));
	int append_val = 1;
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	append_val++;
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	append_val++;
	mrv_append(int_array, &append_val, APPEND_SCALING_INCREMENT);
	append_val++;

	void *two_ptr = mrv_get_idx(int_array, 1);

	append_val = 2;

	MRT_ASSERT(mrv_get_item(int_array, &append_val) == two_ptr,
		   "get middle item");

	append_val = CAFE_BABE;

	MRT_ASSERT(mrv_get_item(int_array, &append_val) == NULL,
		   "item doesnt exist");

	mrv_destroy(int_array);
}

int main(void)
{
	MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
	MrtContext *ctx = mrt_ctx_create(logger);

	// mrs_strings
	MRT_REGISTER_TEST_GROUP(ctx, test_strstr);
	MRT_REGISTER_TEST_GROUP(ctx, test_filter);
	MRT_REGISTER_TEST_GROUP(ctx, test_strcat);
	MRT_REGISTER_TEST_GROUP(ctx, test_pushstr);
	MRT_REGISTER_TEST_GROUP(ctx, test_get_char);
	MRT_REGISTER_TEST_GROUP(ctx, test_setstr);
	MRT_REGISTER_TEST_GROUP(ctx, test_setstrn);
	MRT_REGISTER_TEST_GROUP(ctx, test_get_idx);
	MRT_REGISTER_TEST_GROUP(ctx, test_strchr);
	MRT_REGISTER_TEST_GROUP(ctx, test_strndup);
	MRT_REGISTER_TEST_GROUP(ctx, test_shrink_to_fit);
	MRT_REGISTER_TEST_GROUP(ctx, test_trim_trailing_whitespace);

	// mrv_vectors
	MRT_REGISTER_TEST_GROUP(ctx, test_append);
	MRT_REGISTER_TEST_GROUP(ctx, test_pop);
	MRT_REGISTER_TEST_GROUP(ctx, test_remove);
	MRT_REGISTER_TEST_GROUP(ctx, test_get_pos);
	MRT_REGISTER_TEST_GROUP(ctx, test_get_item_where);
	MRT_REGISTER_TEST_GROUP(ctx, test_get_item);

	Err err = mrt_ctx_run(ctx);

	mrt_ctx_destroy(ctx);
	mrl_destroy(logger);

	return err;
}
