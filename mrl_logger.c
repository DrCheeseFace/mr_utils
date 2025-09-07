#include "mrl_logger.h"
#include <stdio.h>

const char *terminal_color_codes[MRL_SEVERITY_COUNT] = {
	MRL_DEFAULT_COLOR_CODE,
	MRL_BLUE_COLOR_CODE,
	MRL_GREEN_COLOR_CODE,
	MRL_RED_COLOR_CODE,
};

void MRL_reset_severity(void)
{
	printf("%s", terminal_color_codes[MRL_SEVERITY_DEFAULT]);
}

void MRL_set_severity(MRL_Severity severity)
{
	printf("%s", terminal_color_codes[severity]);
}

void MRL_logln(const char *message, MRL_Severity severity)
{
	MRL_set_severity(severity);
	puts(message);
	MRL_reset_severity();
}

void MRL_log(const char *message, MRL_Severity severity)
{
	MRL_set_severity(severity);
	printf("%s", message);
	MRL_reset_severity();
}
