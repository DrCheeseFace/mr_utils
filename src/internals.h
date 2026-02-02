#ifndef INTERNALS_H
#define INTERNALS_H

#include <mr_utils.h>
#include <stdio.h>

struct MrtGroup;

typedef void (*MrtTestFunc)(struct MrtGroup *t_group);

struct MrlLogger {
	FILE *out;
	Bool terminal_color_enabled;
	Bool log_header_enabled;
};

struct MrtGroup {
	MrsString name;
	MrvVector cases;
	size_t pass_count;
	MrtTestFunc func;
};

struct MrtContext {
	MrvVector test_groups;
	struct MrlLogger *logger;
};

#endif // !INTERNALS_H
