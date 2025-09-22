#include "mrl_logger.h"
#include "internals.h"
#include "mrd_debug.h"
#include "mrm_misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

const char *terminal_color_codes[MRL_SEVERITY_COUNT] = {
	MRL_DEFAULT_COLOR_CODE,

	MRL_BLUE_COLOR_CODE,

	MRL_MAGENTA_COLOR_CODE,

	MRL_GREEN_COLOR_CODE,

	MRL_RED_COLOR_CODE,

	MRL_YELLOW_COLOR_CODE,
};

const char *severity_to_log_header[MRL_SEVERITY_COUNT] = { "[LOG]"
							   "[INFO]"
							   "[INFO]"
							   "[OK]"
							   "[ERROR]"
							   "[WARNING]" };

MrlLogger *mrl_create(FILE *out, Bool color, Bool log_header)
{
	struct MrlContext *ctx = malloc(sizeof(struct MrlContext));

	ctx->out = out;
	ctx->terminal_color_enabled = color;
	ctx->log_header_enabled = log_header;

	return ctx;
}

void mrl_destroy(MrlLogger *ctx)
{
	struct MrlContext *mrl_ctx = (struct MrlContext *)ctx;

	if (mrl_ctx->out != NULL) {
		fclose(((struct MrlContext *)ctx)->out);
	}
	free(ctx);
}

void mrl_reset_severity(MrlLogger *ctx)
{
	struct MrlContext *mrl_ctx = (struct MrlContext *)ctx;

	if (mrl_ctx->terminal_color_enabled == TRUE) {
		fprintf(mrl_ctx->out, "%s",
			terminal_color_codes[MRL_SEVERITY_DEFAULT]);
	}
}

void mrl_set_severity(MrlLogger *ctx, MrlSeverity severity)
{
	struct MrlContext *mrl_ctx = (struct MrlContext *)ctx;

	if (mrl_ctx->terminal_color_enabled == TRUE) {
		fprintf(mrl_ctx->out, "%s", terminal_color_codes[severity]);
	}
}

void mrl_logln(MrlLogger *ctx, const char *message, MrlSeverity severity)
{
	struct MrlContext *mrl_ctx = (struct MrlContext *)ctx;

	mrl_set_severity(ctx, severity);

	if (mrl_ctx->log_header_enabled) {
		fprintf(mrl_ctx->out, "%s:", severity_to_log_header[severity]);
		fprintf(mrl_ctx->out, "%ld:", time(NULL));
		mrl_reset_severity(ctx);
	}

	fprintf(mrl_ctx->out, "%s\n", message);

	mrl_reset_severity(ctx);
}

void mrl_log(MrlLogger *ctx, const char *message, MrlSeverity severity)
{
	struct MrlContext *mrl_ctx = (struct MrlContext *)ctx;

	mrl_set_severity(ctx, severity);

	if (mrl_ctx->log_header_enabled) {
		fprintf(mrl_ctx->out, "%s:", severity_to_log_header[severity]);
		fprintf(mrl_ctx->out, "%ld:", time(NULL));
		mrl_reset_severity(ctx);
	}

	fprintf(mrl_ctx->out, "%s", message);

	mrl_reset_severity(ctx);
}
