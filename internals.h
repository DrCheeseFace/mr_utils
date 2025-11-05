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

struct MrtGroup {
	MrsString *description;
	MrvVector cases;
	uint pass_count;
};

struct MrtContext {
	MrvVector test_funcs;
	MrvVector test_groups;
	struct MrlLogger *logger;
};

typedef void (*MrtTestFunc)(struct MrtGroup *t_group);

#endif // !INTERNALS_H
