#include "mrl_logger.h"
#include "mrd_debug.h"
#include <stdio.h>
#include <unistd.h>

const char *terminal_color_codes[MRL_SEVERITY_COUNT] = {
	MRL_DEFAULT_COLOR_CODE,

	MRL_BLUE_COLOR_CODE,

	MRL_MAGENTA_COLOR_CODE,

	MRL_GREEN_COLOR_CODE,

	MRL_RED_COLOR_CODE,

	MRL_YELLOW_COLOR_CODE,
};

void mrl_reset_severity(void)
{
	fprintf(stderr, "%s", terminal_color_codes[MRL_SEVERITY_DEFAULT]);
}

void mrl_set_severity(MrlSeverity severity)
{
	fprintf(stderr, "%s", terminal_color_codes[severity]);
}

void mrl_logln(const char *message, MrlSeverity severity)
{
	mrl_set_severity(severity);
	fprintf(stderr, "%s\n", message);
	mrl_reset_severity();
}

void mrl_log(const char *message, MrlSeverity severity)
{
	mrl_set_severity(severity);
	fprintf(stderr, "%s", message);
	mrl_reset_severity();
}
