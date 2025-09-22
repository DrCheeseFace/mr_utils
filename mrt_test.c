#include "internals.h"
#include "mrd_debug.h"
#include "mrl_logger.h"
#include "mrm_misc.h"
#include "mrs_strings.h"
#include "mrv_vectors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MRT_INIT_TEST_CASES_PER_CONTEXT 64
#define MRT_TAB "    "

typedef struct {
	MrsString description;
	Bool pass;
} MrtCase;

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

struct MrtContext *mrt_ctx_create(const char *description,
				  MrlLogger *logging_ctx)
{
	struct MrtContext *t_ctx = malloc(sizeof(struct MrtContext));
	memset(t_ctx, 0, sizeof(*t_ctx));

	MrsString *s = mrs_create(strlen(description));
	mrs_setstr(s, description, strlen(description));
	t_ctx->description = s;

	mrv_init(&t_ctx->cases, MRT_INIT_TEST_CASES_PER_CONTEXT,
		 sizeof(MrtCase));

	struct MrlLogger *logger = (struct MrlLogger *)logging_ctx;
	t_ctx->logging_context = logger;

	return t_ctx;
}

void mrt_ctx_destroy(struct MrtContext *t_ctx)
{
	for (size_t i = 0; i < t_ctx->cases.len; i++) {
		MrtCase *c = mrv_get_idx(&t_ctx->cases, i);
		mrs_free(&c->description);
	}

	mrv_free(&t_ctx->cases);

	mrs_free(t_ctx->description);
	free(t_ctx->description);

	free(t_ctx);
}

void mrt_ctx_append_case(struct MrtContext *t_ctx, const char *description,
			 Bool pass)
{
	if (pass) {
		t_ctx->pass_count++;
	}

	MrsString s;
	mrs_init(strlen(description), description, strlen(description), &s);

	mrv_append(&t_ctx->cases, &(MrtCase){ .description = s, .pass = pass });
}

Err mrt_ctx_log(struct MrtContext *t_ctx)
{
	mrl_logln(t_ctx->logging_context, "", MRL_SEVERITY_DEFAULT);

	mrl_log(t_ctx->logging_context, "description: ", MRL_SEVERITY_DEFAULT);
	mrl_logln(t_ctx->logging_context, t_ctx->description->value,
		  MRL_SEVERITY_INFO);

	for (size_t i = 0; i < t_ctx->cases.len; i++) {
		mrl_log(t_ctx->logging_context, MRT_TAB, MRL_SEVERITY_DEFAULT);
		MrtCase *c = mrv_get_idx(&t_ctx->cases, i);
		mtr_case_log(t_ctx->logging_context, *c);
	}
	mrl_logln(t_ctx->logging_context, "", MRL_SEVERITY_DEFAULT);

	char pass_rate[15];
	sprintf(pass_rate, "%u/%d Passed", t_ctx->pass_count,
		(int)t_ctx->cases.len);
	mrl_log(t_ctx->logging_context, MRT_TAB, MRL_SEVERITY_DEFAULT);
	mrl_logln(t_ctx->logging_context, pass_rate, MRL_SEVERITY_DEFAULT);

	mrl_log(t_ctx->logging_context, MRT_TAB, MRL_SEVERITY_DEFAULT);
	if (t_ctx->pass_count != t_ctx->cases.len) {
		mrl_logln(t_ctx->logging_context, "FAILED", MRL_SEVERITY_ERROR);
		return ERR;
	} else {
		mrl_logln(t_ctx->logging_context, "PASSED", MRL_SEVERITY_OK);
		return OK;
	}
}
