#include "internals.h"
#include <mr_utils.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MRL_DEFAULT_COLOR_CODE "\x1b[0m"
#define MRL_MAGENTA_COLOR_CODE "\x1b[35m"
#define MRL_BLUE_COLOR_CODE "\x1b[34m"
#define MRL_YELLOW_COLOR_CODE "\x1b[33m"
#define MRL_GREEN_COLOR_CODE "\x1b[32m"
#define MRL_RED_COLOR_CODE "\x1b[31m"

const char *terminal_color_codes[MRL_SEVERITY_COUNT] = {
	MRL_DEFAULT_COLOR_CODE,

	MRL_BLUE_COLOR_CODE,

	MRL_MAGENTA_COLOR_CODE,

	MRL_GREEN_COLOR_CODE,

	MRL_RED_COLOR_CODE,

	MRL_YELLOW_COLOR_CODE,
};

const char *severity_to_log_header[MRL_SEVERITY_COUNT] = {
	"[LOG]", "[INFO]", "[INFO]", "[OK]", "[ERROR]", "[WARNING]"
};

MrlLogger *mrl_create(FILE *out, Bool color, Bool log_header)
{
	struct MrlLogger *ctx = malloc(sizeof(*ctx));
	ctx->out = out;
	ctx->terminal_color_enabled = color;
	ctx->log_header_enabled = log_header;

	return ctx;
}

void mrl_destroy(MrlLogger *ctx)
{
	struct MrlLogger *mrl_ctx = (struct MrlLogger *)ctx;
	if (mrl_ctx->out != NULL) {
		fclose(((struct MrlLogger *)ctx)->out);
	}
	free(ctx);
}

void mrl_reset_severity(MrlLogger *ctx)
{
	struct MrlLogger *mrl_ctx = (struct MrlLogger *)ctx;
	if (mrl_ctx->terminal_color_enabled == TRUE) {
		fprintf(mrl_ctx->out, "%s",
			terminal_color_codes[MRL_SEVERITY_DEFAULT]);
	}
}

void mrl_set_severity(MrlLogger *ctx, MrlSeverity severity)
{
	struct MrlLogger *mrl_ctx = (struct MrlLogger *)ctx;

	if (mrl_ctx->terminal_color_enabled == TRUE) {
		fprintf(mrl_ctx->out, "%s", terminal_color_codes[severity]);
	}
}

mr_internal void mrl_log_header(struct MrlLogger *ctx, MrlSeverity severity)
{
	fprintf(ctx->out, "%s:", severity_to_log_header[severity]);
	fprintf(ctx->out, "%lld: ", (long long)time(NULL));
	mrl_reset_severity(ctx);
}

void mrl_logln(MrlLogger *ctx, MrlSeverity severity, const char *fmt, ...)
{
	struct MrlLogger *mrl_ctx = (struct MrlLogger *)ctx;

	mrl_set_severity(ctx, severity);

	if (mrl_ctx->log_header_enabled) {
		mrl_log_header(ctx, severity);
	}

	va_list args;
	va_start(args, fmt);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
	vfprintf(mrl_ctx->out, fmt, args);
#pragma GCC diagnostic pop

	va_end(args);

	fprintf(mrl_ctx->out, "\n");
	mrl_reset_severity(ctx);
}

void mrl_log(MrlLogger *ctx, MrlSeverity severity, const char *fmt, ...)
{
	struct MrlLogger *mrl_ctx = (struct MrlLogger *)ctx;

	mrl_set_severity(ctx, severity);

	if (mrl_ctx->log_header_enabled) {
		mrl_log_header(ctx, severity);
	}

	va_list args;
	va_start(args, fmt);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
	vfprintf(mrl_ctx->out, fmt, args);
#pragma GCC diagnostic pop

	va_end(args);

	mrl_reset_severity(ctx);
}
