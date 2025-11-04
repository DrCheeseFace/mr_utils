/**
 * this file is a mini testing lib
 */

#ifndef MRT_TEST_H
#define MRT_TEST_H

#include "mrl_logger.h"
#include "mrm_misc.h"
#include <stddef.h>

#define MRT_ASSERT_EQ(expected, actual)                                        \
	(mrt_assert_eq((&expected), (&actual), (sizeof(expected))))

#define MRT_ASSERT(actual)                                                     \
	(mrt_assert_eq(&(bool){ actual }, &(bool){ true }, sizeof(bool)))

#define MRT_ASSERT_NULL(actual) (actual == NULL)

#define MRT_ASSERT_NOT_NULL(actual) (actual != NULL)

/*
 * MrtContext lets you register your test functions for standardised running and logging
 *
 * MrtContext has the following functions
 * \ mrt_ctx_create                - create context. NOTE: requires logger to have "logging_headers" turned off
 * \ mrt_ctx_destroy               - frees itself and contents
 * \ mrt_ctx_run                   - runs and logs registered test functions for context
 * \ mrt_ctx_register_test_func    - registers test functions to run for context
 * \ mrt_group_append_case         - adds test case for test function
 * \
 * \ example usage:
 * \
 * \   void test_strstr(MrtGroup *t_ctx)
 * \   {
 * \           mrt_group_append_case(t_ctx, "11151111111111111231 | 31", 1 == 1);
 * \           return;
 * \   }
 * \
 * \   int main(void)
 * \   {
 * \           MrlLogger *logger = mrl_create(stderr, TRUE, FALSE);
 * \           MrtCtx *ctx = mrt_ctx_create(logger);
 * \
 * \           mrt_ctx_register_test_func(ctx, test_strstr, "test_strstr");
 * \
 * \           Err err = mrt_ctx_run(ctx);
 * \
 * \           mrt_ctx_destroy(ctx);
 * \           mrl_destroy(logger);
 * \
 * \           exit(err);
 * \           return err;
 * \    }
 * \
 */

typedef void MrtCtx;
typedef void MrtGroup;
typedef void (*MrtTestFunc)(MrtGroup *t_group);

MrtCtx *mrt_ctx_create(MrlLogger *logger);

void mrt_ctx_destroy(MrtCtx *ctx);

Err mrt_ctx_run(MrtCtx *ctx);

void mrt_ctx_register_test_func(MrtCtx *ctx, MrtTestFunc test_func,
				const char *description);

void mrt_group_append_case(MrtGroup *t_ctx, const char *description, Bool pass);

Bool mrt_assert_eq(void *expected, void *actual, size_t size_of);

#endif // !MRT_TEST_H
