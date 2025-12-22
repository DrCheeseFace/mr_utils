/**
 * this file is a mini testing lib
 */

#ifndef MRT_TEST_H
#define MRT_TEST_H

#include "internals.h" // IWYU pragma: keep
#include "mrl_logger.h"
#include "mrm_misc.h"
#include <stddef.h>

#define MRT_ASSERT(t_ctx, predicate, desc)                                     \
	internal_mrt_group_append_case(t_ctx, desc, predicate)

/*
 * MrtContext lets you register your test functions for standardised running and logging
 *
 * MrtContext has the following functions
 * \ mrt_ctx_create                - create context. NOTE: requires logger to have "logging_headers" turned off
 * \ mrt_ctx_destroy               - frees itself and contents
 * \ mrt_ctx_run                   - runs and logs registered test functions for context. returns number of failed test groups
 * \ mrt_ctx_register_test_func    - registers test functions to run for context
 * \
 * \ example usage:
 * \
 * \   void test_strstr(MrtGroup *t_ctx)
 * \   {
 * \           MRT_ASSERT(t_ctx, 1 == 1, "this is true");
 * \           MRT_ASSERT(t_ctx, 1 == 2, "this is false");
 * \
 * \           return;
 * \   }
 * \
 * \   void test_filter(MrtGroup *t_ctx)
 * \   {
 * \           MRT_ASSERT(t_ctx, 1 == 1, "this way");
 * \           MRT_ASSERT(t_ctx, 1 == 2, "that way");
 * \
 * \           return;
 * \   }
 * \
 * \   int main(void)
 * \   {
 * \           MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
 * \           MrtCtx *ctx = mrt_ctx_create(logger);
 * \
 * \           mrt_ctx_register_test_func(ctx, test_strstr, "test_strstr");
 * \           mrt_ctx_register_test_func(ctx, test_filter, "test_filter");
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

int mrt_ctx_run(MrtContext *ctx);

void mrt_ctx_register_test_func(MrtContext *ctx,
				void (*test_func)(MrtGroup *t_group),
				const char *description);

Bool mrt_assert_eq(void *expected, void *actual, size_t size_of);

#endif // !MRT_TEST_H
