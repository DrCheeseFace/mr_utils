#ifndef INTERNALS_H
#define INTERNALS_H

#include "mrm_misc.h"
#include "mrs_strings.h"
#include "mrv_vectors.h"
#include <stdio.h>

struct MrlLogger {
	FILE *out;
	Bool terminal_color_enabled;
	Bool log_header_enabled;
};

struct MrtContext {
	MrsString *description;
	MrvVector cases;
	uint pass_count;
	struct MrlLogger *logging_context;
};

#endif // !INTERNALS_H
