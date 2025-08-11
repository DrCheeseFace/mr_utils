#include "../mrs_strings.h"
#include "../mrt_test.h"
#include <stdio.h>
#include <string.h>

int test_mrs_strings_strstr(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_mrs_strings_strstr");

	const char *haystack = "11151111111111111231";
	const char *needle = "31";

	MRS_String *a = MRS_init(0, haystack);

	MRS_String *b = MRS_init(0, needle);

	struct MRT_Case test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 31",
		.pass = MRT_ASSERT_EQ(a->value[18], *MRS_strstr(a, b, 0))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	size_t start_idx = 3;
	test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 31 startidx=3",
		.pass = MRT_ASSERT_EQ(a->value[18],
				      *MRS_strstr(a, b, start_idx))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 31 startidx=NULL",
		.pass = MRT_ASSERT_EQ(a->value[18], *MRS_strstr(a, b, 0))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(b, "52");
	test_case = (struct MRT_Case){
		.description = "11151111111111111231 | 52",
		.pass = MRT_ASSERT_NULL(MRS_strstr(a, b, 0))
	};
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(a);
	MRS_free(b);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_mrs_strings_filter(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_mrs_strings_filter");

	MRS_String *actual = MRS_init(20, "12121");
	MRS_String *expected = MRS_init(20, "111");
	MRS_filter(actual, '2');

	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "12121 | 2",
				   .pass = !MRS_strcmp(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(actual, "2222");
	MRS_setstr(expected, "");
	MRS_filter(actual, '2');
	test_case = (struct MRT_Case){ .description = "2222 | 2",
				       .pass = !MRS_strcmp(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(actual, "2222");
	MRS_setstr(expected, "2222");
	MRS_filter(actual, '3');
	test_case = (struct MRT_Case){ .description = "2222 | 3",
				       .pass = !MRS_strcmp(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(actual);
	MRS_free(expected);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_mrs_strings_strcat(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_mrs_strings_strcat");

	MRS_String *actual = MRS_init(10, "aaabbb");
	MRS_String *append = MRS_init(10, "cc");
	MRS_String *expected = MRS_init(10, "aaabbbcc");
	MRS_strcat(actual, append);

	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "aaabbb | cc",
				   .pass = !MRS_strcmp(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(actual, "123456789");
	MRS_setstr(append, "10");
	test_case =
		(struct MRT_Case){ .description =
					   "123456789 | 10 over capacity",
				   .pass = 0 == !MRS_strcat(actual, append) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(actual, "123456789");
	MRS_setstr(append, "0");
	MRS_setstr(expected, "1234567890");
	MRS_strcat(actual, append);
	test_case = (struct MRT_Case){ .description =
					       "123456789 | 1 exactly capacity",
				       .pass = !MRS_strcmp(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(actual, "123456789");
	MRS_setstr(append, "");
	MRS_setstr(expected, "123456789");
	MRS_strcat(actual, append);
	test_case = (struct MRT_Case){ .description = "123456789 | empty src",
				       .pass = !MRS_strcmp(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(actual, "");
	MRS_setstr(append, "123");
	MRS_setstr(expected, "123");
	MRS_strcat(actual, append);
	test_case = (struct MRT_Case){ .description = "123456789 | empty dest",
				       .pass = !MRS_strcmp(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_setstr(actual, "");
	MRS_setstr(append, "");
	MRS_setstr(expected, "");
	MRS_strcat(actual, append);
	test_case = (struct MRT_Case){ .description = "empty dest and src",
				       .pass = !MRS_strcmp(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(actual);
	MRS_free(expected);
	MRS_free(append);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int test_mrs_strings_get_char(void)
{
	struct MRT_Context *t_ctx = MRT_ctx_create("test_mrs_strings_strcat");

	MRS_String *src = MRS_init(10, "0123456789");

	char actual = MRS_get_char(src, 3);
	char expected = '3';
	struct MRT_Case test_case =
		(struct MRT_Case){ .description = "within bounds fully",
				   .pass = MRT_ASSERT_EQ(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	actual = MRS_get_char(src, 0);
	expected = '0';
	test_case =
		(struct MRT_Case){ .description = "zeroth idx",
				   .pass = MRT_ASSERT_EQ(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	actual = MRS_get_char(src, 9);
	expected = '9';
	test_case =
		(struct MRT_Case){ .description = "last idx",
				   .pass = MRT_ASSERT_EQ(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	actual = MRS_get_char(src, 10);
	expected = '\0';
	test_case =
		(struct MRT_Case){ .description = "outof bounds",
				   .pass = MRT_ASSERT_EQ(expected, actual) };
	MRT_ctx_append_case(t_ctx, test_case);

	MRS_free(src);

	int failed = MRT_ctx_log(t_ctx);
	MRT_ctx_free(t_ctx);
	return failed;
}

int main(void)
{
	int err = 0;
	err = err || test_mrs_strings_strstr();
	err = err || test_mrs_strings_filter();
	err = err || test_mrs_strings_strcat();
	err = err || test_mrs_strings_get_char();
	return err;
}
