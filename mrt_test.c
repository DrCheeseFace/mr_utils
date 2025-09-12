#include "mrd_debug.h"
#include "mrl_logger.h"
#include "mrs_strings.h"
#include "mrv_vectors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MRT_INIT_TEST_CASES_PER_CONTEXT 64
#define MRT_TAB "    "

typedef struct {
	MRS_String description;
	Bool pass;
} MRT_Case;

typedef struct {
	MRS_String *description;
	MRV_Vector cases;
	uint pass_count;
} MRT_Context;

internal void MRT_case_log(MRT_Case test_case)
{
	MRL_log(test_case.description.value, MRL_SEVERITY_DEFAULT);
	if (test_case.pass) {
		MRL_logln(" ... ok", MRL_SEVERITY_OK);
	} else {
		MRL_logln(" ... FAILED", MRL_SEVERITY_ERROR);
	}
	MRL_reset_severity();
}

Bool MRT_assert_eq(void *expected, void *actual, size_t size_of)
{
	return memcmp(expected, actual, size_of) == 0;
}

MRT_Context *MRT_ctx_create(const char *description)
{
	MRT_Context *t_ctx = malloc(sizeof(MRT_Context));
	memset(t_ctx, 0, sizeof(*t_ctx));

	MRS_String *s = MRS_create(strlen(description));
	MRS_setstr(s, description, strlen(description));
	t_ctx->description = s;

	MRV_init(&t_ctx->cases, MRT_INIT_TEST_CASES_PER_CONTEXT,
		 sizeof(MRT_Case));

	return t_ctx;
}

void MRT_ctx_destroy(MRT_Context *t_ctx)
{
	for (size_t i = 0; i < t_ctx->cases.len; i++) {
		MRT_Case *c = MRV_get_idx(&t_ctx->cases, i);
		MRS_free(&c->description);
	}

	MRV_free(&t_ctx->cases);

	MRS_free(t_ctx->description);
	free(t_ctx->description);

	free(t_ctx);
}

void MRT_ctx_append_case(MRT_Context *t_ctx, const char *description, Bool pass)
{
	if (pass) {
		t_ctx->pass_count++;
	}

	MRS_String s;
	MRS_init(strlen(description), description, strlen(description), &s);

	MRV_append(&t_ctx->cases,
		   &(MRT_Case){ .description = s, .pass = pass });
}

Err MRT_ctx_log(MRT_Context *t_ctx)
{
	MRL_logln("", MRL_SEVERITY_DEFAULT);

	MRL_log("description: ", MRL_SEVERITY_DEFAULT);
	MRL_logln(t_ctx->description->value, MRL_SEVERITY_INFO);

	for (size_t i = 0; i < t_ctx->cases.len; i++) {
		MRL_log(MRT_TAB, MRL_SEVERITY_DEFAULT);
		MRT_Case *c = MRV_get_idx(&t_ctx->cases, i);
		MRT_case_log(*c);
	}
	MRL_logln("", MRL_SEVERITY_DEFAULT);

	char pass_rate[15];
	sprintf(pass_rate, "%d/%d Passed", t_ctx->pass_count,
		(int)t_ctx->cases.len);
	MRL_log(MRT_TAB, MRL_SEVERITY_DEFAULT);
	MRL_logln(pass_rate, MRL_SEVERITY_DEFAULT);

	MRL_log(MRT_TAB, MRL_SEVERITY_DEFAULT);
	if (t_ctx->pass_count != t_ctx->cases.len) {
		MRL_logln("FAILED", MRL_SEVERITY_ERROR);
		return ERR;
	} else {
		MRL_logln("PASSED", MRL_SEVERITY_OK);
		return OK;
	}
}
