#include "mrl_logger.h"
#include "mrd_debug.h"
#include "mrm_misc.h"
#include <stdio.h>
#include <unistd.h>

FILE *output_stream = NULL;

const char *terminal_color_codes[MRL_SEVERITY_COUNT] = {
	MRL_DEFAULT_COLOR_CODE,

	MRL_BLUE_COLOR_CODE,

	MRL_MAGENTA_COLOR_CODE,

	MRL_GREEN_COLOR_CODE,

	MRL_RED_COLOR_CODE,

	MRL_YELLOW_COLOR_CODE,
};

internal void mrl_init(void)
{
	output_stream = stderr;
}

void mrl_set_output_stream(FILE *out)
{
	output_stream = out;
}

void mrl_reset_severity(void)
{
	if (output_stream == NULL) {
		mrl_init();
	}
	fprintf(output_stream, "%s",
		terminal_color_codes[MRL_SEVERITY_DEFAULT]);
}

void mrl_set_severity(MrlSeverity severity)
{
	fprintf(output_stream, "%s", terminal_color_codes[severity]);
}

void mrl_logln(const char *message, MrlSeverity severity)
{
	if (output_stream == NULL) {
		mrl_init();
	}
	mrl_set_severity(severity);
	fprintf(output_stream, "%s\n", message);
	mrl_reset_severity();
}

void mrl_log(const char *message, MrlSeverity severity)
{
	if (output_stream == NULL) {
		mrl_init();
	}
	mrl_set_severity(severity);
	fprintf(output_stream, "%s", message);
	mrl_reset_severity();
}
