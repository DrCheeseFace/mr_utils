/*
 * this file is a mini testing lib
 */

#ifndef MRT_TEST_H
#define MRT_TEST_H

#include "mrm_misc.h"
#include <stddef.h>

#define MRT_ASSERT_EQ(expected, actual)                                        \
	(mrt_assert_eq((&expected), (&actual), (sizeof(expected))))

#define MRT_ASSERT(actual)                                                     \
	(mrt_assert_eq(&(bool){ actual }, &(bool){ true }, sizeof(bool)))

#define MRT_ASSERT_NULL(actual) (actual == NULL)

#define MRT_ASSERT_NOT_NULL(actual) (actual != NULL)

/*
 * MrtContext holds test cases and a description for the group of test cases
 *
 * MrtContext has the following functions
 * \ mrt_ctx_create         - create context. NOTE: requires logger to have "logging_headers" turned off
 * \ mrt_ctx_destroy        - frees itself and contents
 * \ mrt_ctx_append_case    - adds test case to context
 * \ mrt_ctx_log            - logs context and test case results within context
 * \
 * \ eg:
 * \
 * \     MrtContext *t_ctx = mrt_ctx_create("my test context description");
 * \
 * \     mrt_ctx_append_case(t_ctx, "my test description 1", 1 == 2);
 * \     mrt_ctx_append_case(t_ctx, "my test description 2", 1 == 1);
 * \
 * \     Err err = mrt_ctx_log(t_ctx);
 * \
 * \     mrt_ctx_destroy(t_ctx);
 * \
 * \     return err;
 */
typedef void MrtContext;

MrtContext *mrt_ctx_create(const char *description, MrtContext *logging_ctx);

void mrt_ctx_destroy(MrtContext *t_ctx);

void mrt_ctx_append_case(MrtContext *t_ctx, const char *description, Bool pass);

/*
 * `returns` 0 if passed
 */
Err mrt_ctx_log(MrtContext *t_ctx);

Bool mrt_assert_eq(void *expected, void *actual, size_t size_of);

#endif // !MRT_TEST_H
