# my C utils lib

- `mrs_strings.h`: string manipulation library
- `mrv_vector.h`: vector library
- `mrt_test.h`: mini test library
- `mrl_logger.h`: logging utilities
- `mrd_debug.h`: memory allocation debugging info. enable by defining DEBUG eg: `#define DEBUG` or the `-DDEBUG` gcc flag
- `mrm_misc.h`: useful defines and cafebabe!

#### mrd_debug example build
```Makefile
CC          = gcc
CSTANDARD   = c99

INCLUDES    = -Iinclude

WARNINGS  = -Wall -Wextra -Werror -Wpedantic -pedantic-errors
WARNINGS += -Wpointer-arith -Wcast-align -Wwrite-strings
WARNINGS += -Wstrict-prototypes
WARNINGS += -Wswitch-default -Wswitch-enum -Wunreachable-code
WARNINGS += -Wbad-function-cast -Wcast-qual -Wundef
WARNINGS += -Wshadow -Wfloat-equal -Wformat=2
WARNINGS += -Wredundant-decls -Wnested-externs

ifneq (,$(filter debug build-debug,$(MAKECMDGOALS)))
    BUILD_TYPE := debug
    CFLAGS     := -O0 -g -fno-omit-frame-pointer -rdynamic -DDEBUG -DMRD_DEBUG_DEFAULT $(WARNINGS) $(INCLUDES)
else
    BUILD_TYPE := release
    CFLAGS     := -O2 $(WARNINGS) $(INCLUDES)
endif

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/$(BUILD_TYPE)

TARGET_TEST    = $(OBJ_DIR)/test.out
TARGET_SPACERS = $(OBJ_DIR)/spacers

SRC_COMMON     = $(wildcard *.c)
SRC_TEST       = $(wildcard test/*.c)
SRC_TOOLS      = $(wildcard tools/*.c)

OBJ_COMMON     = $(SRC_COMMON:%.c=$(OBJ_DIR)/%.o)
OBJ_TEST       = $(SRC_TEST:%.c=$(OBJ_DIR)/%.o)
OBJ_TOOLS      = $(SRC_TOOLS:%.c=$(OBJ_DIR)/%.o)

ALL_TEST_OBJS  = $(OBJ_COMMON) $(OBJ_TEST)
ALL_SPACERS_OBJS = $(OBJ_COMMON) $(OBJ_TOOLS)

.PHONY: all test run clean format format-check bear debug build-debug

all: $(TARGET_TEST)

$(TARGET_TEST): $(ALL_TEST_OBJS)
	$(CC) $(ALL_TEST_OBJS) -o $@

$(TARGET_SPACERS): $(ALL_SPACERS_OBJS)
	$(CC) $(ALL_SPACERS_OBJS) -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ -std=$(CSTANDARD) $(CFLAGS)

test: $(TARGET_TEST)
	./$(TARGET_TEST)

run: test

build-debug: $(TARGET_TEST)

debug: build-debug
	./$(TARGET_TEST)

clean:
	rm -rf $(BUILD_DIR)
	rm -f compile_commands.json

bear: clean
	bear -- make build-debug

format: $(TARGET_SPACERS)
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i --verbose
	git ls-files | xargs  $(TARGET_SPACERS)

format-check: $(TARGET_SPACERS)
	find . -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror --verbose
	git ls-files | xargs  $(TARGET_SPACERS)

-include $(ALL_TEST_OBJS:.o=.d)
-include $(ALL_SPACERS_OBJS:.o=.d)
```
- note: -DDEBUG enables mrd_debug
- note: -DMRD_DEBUG_DEFAULT sets log level of mrd_debug. see `mrd_debug.h` for options
- note: `mrd_debug.h` requires the build flag `-rdynamic` do show backtrace symbols
- note: `-rdynamic` breaks `-fsanitize=address`
- note: `static` functions wont be displayed as the symbol isnt exported :(




### TODO
- [x] REWRITE spacers.c IT BAD VERY BAD
- [x] log to defined out
- [x] dynamic array lib
- [x] whitespace remover
- [x] debug maintain only active allocations. eg: freed and not realloced to something not freed
- [x] add left rigth comparison for failed tests
- [ ] add ability to define locked regions for logger library
- [ ] malloc calloc realloc free wrappers for debugging memory leaks eg: this PTR (id) with this DATA that was allocated HERE, was not freed
- [ ] optimize mrd_debug. map offsets to log messages so i dont have to calculate multiple times
- [ ] useful runtime error message. something with coredump file abort()
- [ ] mrv_append, have flags for how to realloc if needed. double, log, increment etc


i hope you like UB!


![https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif](https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif)
