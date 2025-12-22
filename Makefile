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
