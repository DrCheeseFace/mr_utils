#ifndef MRT_TEST_H
#define MRT_TEST_H

#include <stdbool.h>
#include <sys/types.h>

#define MRT_DEFAULT_COLOR_CODE "\x1b[0m"
#define MRT_BLUE_COLOR_CODE "\x1b[34m"
#define MRT_GREEN_COLOR_CODE "\x1b[32m"
#define MRT_RED_COLOR_CODE "\x1b[31m"

#define MRT_MAX_TEST_CASES_PER_CONTEXT 64
#define MRT_MAX_CONTEXT_DESCRIPTION 64

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

typedef enum {
	MRT_SEVERITY_DEFAULT,
	MRT_SEVERITY_INFO,
	MRT_SEVERITY_OK,
	MRT_SEVERITY_ERROR,
	MRT_SEVERITY_COUNT
} MRT_Severity;

bool MRT_assert_eq(void *expected, void *actual, size_t size_of);

struct MRT_Context *MRT_ctx_create(const char *description);

void MRT_ctx_free(struct MRT_Context *t_ctx);

void MRT_ctx_append_case(struct MRT_Context *t_ctx, struct MRT_Case test_case);

/*
 * `returns` 0 if passed 
 */
int MRT_ctx_log(struct MRT_Context *t_ctx);

#endif // !MRT_TEST_H
