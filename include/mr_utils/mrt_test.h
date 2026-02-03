/**
 * this file is a mini testing lib
 */

#ifndef MRT_TEST_H
#define MRT_TEST_H

#include <mr_utils.h>
#include <stddef.h>

/*
 * MrtContext lets you register your test functions for standardised running and logging
 *
 * MrtContext has the following functions
 * \ mrt_ctx_create                - create context. NOTE: requires logger to have "logging_headers" turned off
 * \ mrt_ctx_destroy               - frees itself and contents
 * \ mrt_ctx_run                   - runs and logs registered test functions for context. returns number of failed test groups
 * \
 * \ MRT_REGISTER_TEST_GROUP       - registers a test group to a test context
 * \ MRT_TEST_GROUP                - declares a test group function
 * \ MRT_ASSERT                    - used inside of a MRT_TEST_GROUP to make assertions
 * \
 * \
 * \ example usage:
 * \
 * \   MRT_TEST_GROUP(test_strstr)
 * \   {
 * \           MRT_ASSERT(1 == 1, "this is true");
 * \           MRT_ASSERT(1 == 2, "this is false");
 * \
 * \           return;
 * \   }
 * \
 * \   MRT_TEST_GROUP(test_filter)
 * \   {
 * \           MRT_ASSERT(1 == 1, "this way");
 * \           MRT_ASSERT(1 == 2, "that way");
 * \
 * \           return;
 * \   }
 * \
 * \   int main(void)
 * \   {
 * \           MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
 * \           MrtCtx *ctx = mrt_ctx_create(logger);
 * \
 * \           MRT_REGISTER_TEST_GROUP(ctx, test_strstr);
 * \           MRT_REGISTER_TEST_GROUP(ctx, test_filter);
 * \
 * \           int group_err_count = mrt_ctx_run(ctx); // returns number of failed test groups
 * \
 * \           mrt_ctx_destroy(ctx);
 * \           mrl_destroy(logger);
 * \
 * \           return group_err_count;
 * \    }
 * \
 */

typedef void MrtContext;
typedef void MrtGroup;

MrtContext *mrt_ctx_create(MrlLogger *logger);
void mrt_ctx_destroy(MrtContext *ctx);

int mrt_ctx_run(MrtContext *ctx, Bool run_tests_parrallelized);

#define MRT_TEST_GROUP(name) void name(MrtGroup *t_ctx)

#define MRT_REGISTER_TEST_GROUP(t_ctx, test_func)                              \
	mrt_ctx_register_test_func(t_ctx, test_func, #test_func)
void mrt_ctx_register_test_func(MrtContext *ctx,
				void (*test_func)(MrtGroup *t_group),
				const char *test_func_name);

#define MRT_ASSERT(predicate, desc)                                            \
	mrt_group_append_case(t_ctx, desc, predicate, #predicate, __FILE__,    \
			      __LINE__)
void mrt_group_append_case(MrtGroup *t_ctx, const char *description, Bool pass,
			   const char *predicate, const char *file_name,
			   int line_number);

#endif // !MRT_TEST_H
