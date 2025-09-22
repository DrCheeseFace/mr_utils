/*
 * this file has logging utilities
 *
 * default output stream is set to `stderr`
 *
 * \ mrl_create            - create logging context
 * \ mrl_destroy           - destroy logging context
 * \ mrl_reset_severity    - sets log severity to MRL_SEVERITY_DEFAULT
 * \ mrl_set_severity      - sets log severity
 * \ mrl_log               - log line to stderr
 * \ mrl_logln             - log lint to stderr with newline
 *
 */

#ifndef MRL_LOGGER_H
#define MRL_LOGGER_H

#include "mrm_misc.h"
#include <stdio.h>
#define MRL_DEFAULT_COLOR_CODE "\x1b[0m"
#define MRL_MAGENTA_COLOR_CODE "\x1b[35m"
#define MRL_BLUE_COLOR_CODE "\x1b[34m"
#define MRL_YELLOW_COLOR_CODE "\x1b[33m"
#define MRL_GREEN_COLOR_CODE "\x1b[32m"
#define MRL_RED_COLOR_CODE "\x1b[31m"

typedef enum {
	MRL_SEVERITY_DEFAULT,
	MRL_SEVERITY_INFO,
	MRL_SEVERITY_ALT_INFO,
	MRL_SEVERITY_OK,
	MRL_SEVERITY_ERROR,
	MRL_SEVERITY_WARNING,
	MRL_SEVERITY_COUNT
} MrlSeverity;

typedef void MrlLogger;

MrlLogger *mrl_create(FILE *out, Bool color, Bool log_header);

void mrl_destroy(MrlLogger *ctx);

void mrl_reset_severity(MrlLogger *ctx);

void mrl_set_severity(MrlLogger *ctx, MrlSeverity severity);

void mrl_logln(MrlLogger *ctx, const char *message, MrlSeverity severity);

void mrl_log(MrlLogger *ctx, const char *message, MrlSeverity severity);

#endif // !MRL_LOGGER_H
