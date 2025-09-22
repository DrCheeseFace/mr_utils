#ifndef INTERNALS_H
#define INTERNALS_H

#include "mrm_misc.h"
#include <stdio.h>

struct MrlContext {
	FILE *out;
	Bool terminal_color_enabled;
	Bool log_header_enabled;
};

#endif // !INTERNALS_H
