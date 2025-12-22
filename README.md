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

CFLAGS      = -O2 $(WARNINGS) $(INCLUDES)
DEBUG_FLAGS = -O0 -g -fno-omit-frame-pointer -rdynamic -DDEBUG -DMRD_DEBUG_DEFAULT

TARGET_TEST    = test.out
TARGET_SPACERS = ./spacers

SRC_COMMON     = $(wildcard *.c)
SRC_TEST       = $(wildcard test/*.c)
SRC_TOOLS      = $(wildcard tools/*.c)

OBJ_COMMON     = $(SRC_COMMON:.c=.o)
OBJ_TEST       = $(SRC_TEST:.c=.o)
OBJ_TOOLS      = $(SRC_TOOLS:.c=.o)

ALL_TEST_OBJS  = $(OBJ_COMMON) $(OBJ_TEST)
ALL_SPACERS_OBJS = $(OBJ_COMMON) $(OBJ_TOOLS)

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(CC) -MD -c $< -o $@ -std=$(CSTANDARD) $(CFLAGS)

.PHONY: all test run clean format format-check bear debug build-debug

all: $(TARGET_TEST)

$(TARGET_TEST): $(ALL_TEST_OBJS)
	$(CC) $(ALL_TEST_OBJS) -o $@

$(TARGET_SPACERS): $(ALL_SPACERS_OBJS)
	$(CC) $(ALL_SPACERS_OBJS) -o $@

test: $(TARGET_TEST)
	./$(TARGET_TEST)

run: test

build-debug: CFLAGS := $(DEBUG_FLAGS) $(WARNINGS) $(INCLUDES)
build-debug: $(TARGET_TEST)

debug: build-debug
	./$(TARGET_TEST)

clean:
	rm -f $(TARGET_TEST) $(TARGET_SPACERS)
	rm -f *.o test/*.o tools/*.o
	rm -f *.d test/*.d tools/*.d
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
- [x] log to defined out
- [x] dynamic array lib
- [x] whitespace remover
- [x] debug maintain only active allocations. eg: freed and not realloced to something not freed
- [ ] add left rigth comparison for failed tests
- [ ] malloc calloc realloc free wrappers for debugging memory leaks eg: this PTR (id) with this DATA that was allocated HERE, was not freed
- [ ] optimize mrd_debug. map offsets to log messages so i dont have to calculate multiple times
- [ ] useful runtime error message. something with coredump file abort()


i hope you like UB!


![https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif](https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif)
