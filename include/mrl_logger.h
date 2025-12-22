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

#include <mrm_misc.h>
#include <stdio.h>

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

void mrl_logln(MrlLogger *ctx, MrlSeverity severity, const char *fmt, ...);

void mrl_log(MrlLogger *ctx, MrlSeverity severity, const char *fmt, ...);

#endif // !MRL_LOGGER_H
