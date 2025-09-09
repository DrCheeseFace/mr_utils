#ifndef MRT_TEST_H
#define MRT_TEST_H

#include "mrm_misc.h"
#include <sys/types.h>

#define MRT_MAX_TEST_CASES_PER_CONTEXT 128
#define MRT_MAX_CONTEXT_DESCRIPTION 128

#define MRT_TAB "    "

#define MRT_ASSERT_EQ(expected, actual)                                        \
	(MRT_assert_eq((&expected), (&actual), (sizeof(expected))))

#define MRT_ASSERT(actual)                                                     \
	(MRT_assert_eq(&(bool){ actual }, &(bool){ true }, sizeof(bool)))

#define MRT_ASSERT_NULL(actual) (actual == NULL)

#define MRT_ASSERT_NOT_NULL(actual) (actual != NULL)

struct MRT_Case {
	char description[MRT_MAX_CONTEXT_DESCRIPTION];
	bool pass;
};

struct MRT_Context {
	char description[MRT_MAX_CONTEXT_DESCRIPTION];
	int pass_count;
	int fail_count;
	struct MRT_Case cases[MRT_MAX_TEST_CASES_PER_CONTEXT];
	int case_count;
};

bool MRT_assert_eq(void *expected, void *actual, size_t size_of);

struct MRT_Context *MRT_ctx_create(const char *description);

void MRT_ctx_free(struct MRT_Context *t_ctx);

void MRT_ctx_append_case(struct MRT_Context *t_ctx, struct MRT_Case test_case);

/*
 * `returns` 0 if passed 
 */
int MRT_ctx_log(struct MRT_Context *t_ctx);

#endif // !MRT_TEST_H
