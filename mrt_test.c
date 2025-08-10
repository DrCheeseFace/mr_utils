#include "mrt_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *terminal_color_codes[MRT_SEVERITY_COUNT] = {
	MRT_DEFAULT_COLOR_CODE,
	MRT_BLUE_COLOR_CODE,
	MRT_GREEN_COLOR_CODE,
	MRT_RED_COLOR_CODE,
};

void MRT_reset_severity(void)
{
	printf("%s", terminal_color_codes[MRT_SEVERITY_DEFAULT]);
}

void MRT_set_severity(MRT_Severity severity)
{
	printf("%s", terminal_color_codes[severity]);
}

void MRT_logln(const char *message, MRT_Severity severity)
{
	MRT_set_severity(severity);
	puts(message);
	MRT_reset_severity();
}

void MRT_log(const char *message, MRT_Severity severity)
{
	MRT_set_severity(severity);
	printf("%s", message);
	MRT_reset_severity();
}

bool MRT_assert_eq(void *expected, void *actual, size_t size_of)
{
	return memcmp(expected, actual, size_of) == 0;
}

void MRT_case_log(struct MRT_Case test_case)
{
	MRT_log(test_case.description, MRT_SEVERITY_DEFAULT);
	if (test_case.pass) {
		MRT_logln(" PASS", MRT_SEVERITY_OK);
	} else {
		MRT_logln(" FAILED", MRT_SEVERITY_ERROR);
	}
	MRT_reset_severity();
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
	MRT_logln("", MRT_SEVERITY_DEFAULT);

	MRT_log("description: ", MRT_SEVERITY_DEFAULT);
	MRT_logln(t_ctx->description, MRT_SEVERITY_INFO);

	for (int i = 0; i < t_ctx->case_count; i++) {
		MRT_log(MRT_TAB, MRT_SEVERITY_DEFAULT);
		MRT_case_log(t_ctx->cases[i]);
	}
	MRT_logln("", MRT_SEVERITY_DEFAULT);

	char pass_rate[15];
	sprintf(pass_rate, "%d/%d Passed", t_ctx->pass_count,
		t_ctx->case_count);
	MRT_log(MRT_TAB, MRT_SEVERITY_DEFAULT);
	MRT_logln(pass_rate, MRT_SEVERITY_DEFAULT);

	MRT_log(MRT_TAB, MRT_SEVERITY_DEFAULT);
	if (t_ctx->fail_count != 0) {
		MRT_logln("FAILED", MRT_SEVERITY_ERROR);
		return 1;
	} else {
		MRT_logln("PASSED", MRT_SEVERITY_OK);
		return 0;
	}
}
