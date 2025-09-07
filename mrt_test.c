#include "mrt_test.h"
#include "mrl_logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool MRT_assert_eq(void *expected, void *actual, size_t size_of)
{
	return memcmp(expected, actual, size_of) == 0;
}

void MRT_case_log(struct MRT_Case test_case)
{
	MRL_log(test_case.description, MRL_SEVERITY_DEFAULT);
	if (test_case.pass) {
		MRL_logln(" ... ok", MRL_SEVERITY_OK);
	} else {
		MRL_logln(" ... FAILED", MRL_SEVERITY_ERROR);
	}
	MRL_reset_severity();
}

struct MRT_Context *MRT_ctx_create(const char *description)
{
	struct MRT_Context *t_ctx = malloc(sizeof(struct MRT_Context));
	memset(t_ctx, 0, sizeof(*t_ctx));
	strncpy(t_ctx->description, description,
		MRT_MAX_CONTEXT_DESCRIPTION - 1);
	return t_ctx;
}

void MRT_ctx_free(struct MRT_Context *t_ctx)
{
	free(t_ctx);
}

void MRT_ctx_append_case(struct MRT_Context *t_ctx, struct MRT_Case test_case)
{
	if (test_case.pass) {
		t_ctx->pass_count++;
	} else {
		t_ctx->fail_count++;
	}

	t_ctx->cases[t_ctx->case_count] = test_case;
	t_ctx->case_count++;
}

int MRT_ctx_log(struct MRT_Context *t_ctx)
{
	MRL_logln("", MRL_SEVERITY_DEFAULT);

	MRL_log("description: ", MRL_SEVERITY_DEFAULT);
	MRL_logln(t_ctx->description, MRL_SEVERITY_INFO);

	for (int i = 0; i < t_ctx->case_count; i++) {
		MRL_log(MRT_TAB, MRL_SEVERITY_DEFAULT);
		MRT_case_log(t_ctx->cases[i]);
	}
	MRL_logln("", MRL_SEVERITY_DEFAULT);

	char pass_rate[15];
	sprintf(pass_rate, "%d/%d Passed", t_ctx->pass_count,
		t_ctx->case_count);
	MRL_log(MRT_TAB, MRL_SEVERITY_DEFAULT);
	MRL_logln(pass_rate, MRL_SEVERITY_DEFAULT);

	MRL_log(MRT_TAB, MRL_SEVERITY_DEFAULT);
	if (t_ctx->fail_count != 0) {
		MRL_logln("FAILED", MRL_SEVERITY_ERROR);
		return 1;
	} else {
		MRL_logln("PASSED", MRL_SEVERITY_OK);
		return 0;
	}
}
