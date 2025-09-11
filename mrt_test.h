/*
 * this file is a mini testing lib
 */

#ifndef MRT_TEST_H
#define MRT_TEST_H

#include "mrm_misc.h"
#include <stddef.h>

#define MRT_INIT_TEST_CASES_PER_CONTEXT 64

#define MRT_TAB "    "

#define MRT_ASSERT_EQ(expected, actual)                                        \
	(MRT_assert_eq((&expected), (&actual), (sizeof(expected))))

#define MRT_ASSERT(actual)                                                     \
	(MRT_assert_eq(&(bool){ actual }, &(bool){ true }, sizeof(bool)))

#define MRT_ASSERT_NULL(actual) (actual == NULL)

#define MRT_ASSERT_NOT_NULL(actual) (actual != NULL)

/*
 * MRT_Context holds test cases and a description for the group of test cases 
 *
 */
typedef struct MRT_Context MRT_Context;

Bool MRT_assert_eq(void *expected, void *actual, size_t size_of);

MRT_Context *MRT_ctx_create(const char *description);

void MRT_ctx_free(MRT_Context *t_ctx);

void MRT_ctx_append_case(MRT_Context *t_ctx, const char *description,
			 Bool pass);

/*
 * `returns` 0 if passed 
 */
int MRT_ctx_log(struct MRT_Context *t_ctx);

#endif // !MRT_TEST_H
