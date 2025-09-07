#ifndef MRL_LOGGER_H
#define MRL_LOGGER_H

#define MRL_DEFAULT_COLOR_CODE "\x1b[0m"
#define MRL_CYAN_COLOR_CODE "\x1b[36m"
#define MRL_MAGENTA_COLOR_CODE "\x1b[35m"
#define MRL_BLUE_COLOR_CODE "\x1b[34m"
#define MRL_YELLOW_COLOR_CODE "\x1b[33m"
#define MRL_GREEN_COLOR_CODE "\x1b[32m"
#define MRL_RED_COLOR_CODE "\x1b[31m"

typedef enum {
	MRL_SEVERITY_DEFAULT,
	MRL_SEVERITY_INFO,
	MRL_SEVERITY_ALT_INFO,
	MRL_SEVERITY_ALT_ALT_INFO,
	MRL_SEVERITY_OK,
	MRL_SEVERITY_ERROR,
	MRL_SEVERITY_WARNING,
	MRL_SEVERITY_COUNT
} MRL_Severity;

void MRL_reset_severity(void);

void MRL_set_severity(MRL_Severity severity);

void MRL_logln(const char *message, MRL_Severity severity);

void MRL_log(const char *message, MRL_Severity severity);

#endif // !MRL_LOGGER_H
