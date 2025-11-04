#include "internals.h"
#include "mrd_debug.h"
#include "mrl_logger.h"
#include "mrm_misc.h"
#include "mrs_strings.h"
#include "mrv_vectors.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MRT_INIT_TEST_CASES_PER_CONTEXT 64
#define MRT_TAB "    "

typedef struct {
	MrsString description;
	Bool pass;
} MrtCase;

internal Err mrt_group_log(struct MrtGroup *t_group, struct MrlLogger *logger);
internal void mrt_group_destroy(struct MrtGroup *t_group);

internal void mtr_case_log(MrlLogger *mrl_ctx, MrtCase test_case)
{
	mrl_log(mrl_ctx, test_case.description.value, MRL_SEVERITY_DEFAULT);
	if (test_case.pass) {
		mrl_logln(mrl_ctx, " ... ok", MRL_SEVERITY_OK);
	} else {
		mrl_logln(mrl_ctx, " ... FAILED", MRL_SEVERITY_ERROR);
	}
	mrl_reset_severity(mrl_ctx);
}

Bool mrt_assert_eq(void *expected, void *actual, size_t size_of)
{
	return memcmp(expected, actual, size_of) == 0;
}

internal void mrt_group_init(struct MrtGroup *t_group, const char *description)
{
	memset(t_group, 0, sizeof(*t_group));

	MrsString *s = mrs_create(strlen(description));
	mrs_setstr(s, description, strlen(description));
	t_group->description = s;

	mrv_init(&t_group->cases, MRT_INIT_TEST_CASES_PER_CONTEXT,
		 sizeof(MrtCase));
}

struct MrtCtx *mrt_ctx_create(MrlLogger *logger)
{
	struct MrtCtx *t_ctx = malloc(sizeof(*t_ctx));
	memset(t_ctx, 0, sizeof(*t_ctx));

	mrv_init(&t_ctx->test_funcs, 64, sizeof(MrtTestFunc));
	mrv_init(&t_ctx->test_groups, 64, sizeof(struct MrtGroup *));

	t_ctx->logger = logger;

	return t_ctx;
}

void mrt_ctx_destroy(struct MrtCtx *ctx)
{
	for (uint i = 0; i < ctx->test_groups.len; i++) {
		struct MrtGroup *t_group =
			*(struct MrtGroup **)mrv_get_idx(&ctx->test_groups, i);
		mrt_group_destroy(t_group);
	}

	mrv_free(&ctx->test_funcs);
	mrv_free(&ctx->test_groups);

	free(ctx);
}

void mrt_ctx_register_test_func(struct MrtCtx *ctx, MrtTestFunc t_func,
				const char *description)
{
	mrv_append(&ctx->test_funcs, &t_func);

	struct MrtGroup *t_group = malloc(sizeof(struct MrtGroup));

	mrt_group_init(t_group, description);

	mrv_append(&ctx->test_groups, &t_group);

	return;
}

Err mrt_ctx_run(struct MrtCtx *ctx)
{
	Err err = OK;

	for (uint i = 0; i < ctx->test_funcs.len; i++) {
		MrtTestFunc *test_func =
			(MrtTestFunc *)mrv_get_idx(&ctx->test_funcs, i);

		struct MrtGroup *t_group =
			*(struct MrtGroup **)mrv_get_idx(&ctx->test_groups, i);

		(*test_func)(t_group);

		err = err || mrt_group_log(t_group, ctx->logger);
	}

	return err;
}

internal void mrt_group_destroy(struct MrtGroup *t_group)
{
	for (size_t i = 0; i < t_group->cases.len; i++) {
		MrtCase *c = mrv_get_idx(&t_group->cases, i);
		mrs_free(&c->description);
	}

	mrv_free(&t_group->cases);

	mrs_free(t_group->description);
	free(t_group->description);

	free(t_group);
}

void mrt_group_append_case(struct MrtGroup *t_group, const char *description,
			   Bool pass)
{
	if (pass) {
		t_group->pass_count++;
	}

	MrsString s;
	mrs_init(strlen(description), description, strlen(description), &s);

	mrv_append(&t_group->cases,
		   &(MrtCase){ .description = s, .pass = pass });
}

internal Err mrt_group_log(struct MrtGroup *t_group, struct MrlLogger *logger)
{
	mrl_logln(logger, "", MRL_SEVERITY_DEFAULT);

	mrl_log(logger, "description: ", MRL_SEVERITY_DEFAULT);
	mrl_logln(logger, t_group->description->value, MRL_SEVERITY_INFO);

	for (size_t i = 0; i < t_group->cases.len; i++) {
		mrl_log(logger, MRT_TAB, MRL_SEVERITY_DEFAULT);
		MrtCase *c = mrv_get_idx(&t_group->cases, i);
		mtr_case_log(logger, *c);
	}
	mrl_logln(logger, "", MRL_SEVERITY_DEFAULT);

	char pass_rate[15];
	sprintf(pass_rate, "%u/%d Passed", t_group->pass_count,
		(int)t_group->cases.len);
	mrl_log(logger, MRT_TAB, MRL_SEVERITY_DEFAULT);
	mrl_logln(logger, pass_rate, MRL_SEVERITY_DEFAULT);

	mrl_log(logger, MRT_TAB, MRL_SEVERITY_DEFAULT);
	if (t_group->pass_count != t_group->cases.len) {
		mrl_logln(logger, "FAILED", MRL_SEVERITY_ERROR);
		return ERR;
	} else {
		mrl_logln(logger, "PASSED", MRL_SEVERITY_OK);
		return OK;
	}
}
