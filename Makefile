CC = gcc

CSTANDARD = c99

CFLAGS_TEST = -Wall -Wextra -Werror -Wpointer-arith -Wcast-align \
         -Wstrict-prototypes -Wwrite-strings -Waggregate-return \
         -Wswitch-default -Wswitch-enum -Wunreachable-code \
	 -Wunused-parameter -Wuninitialized -Winit-self -Wpedantic \
         -pedantic-errors -Wbad-function-cast -Wcast-align\
	 -Wformat=2 -Wlogical-op   -Wmissing-include-dirs \
         -Wredundant-decls -Wsequence-point -Wshadow \
	 -Wswitch -Wundef -Wunused-but-set-parameter \
	 -Wcast-qual  -Wfloat-equal -Wnested-externs \
	 -O0 -std=$(CSTANDARD) -g \
	 -fsanitize=address \
	 
	 
CFLAGS = -Wall -Wextra -Werror \
	 -O2 -std=$(CSTANDARD)\

TEST_TARGET = test.out

TEST_SRC = *.c test/*.c

DEBUG_ENABLED ?= 1

ifeq ($(DEBUG_ENABLED), 1)
    CFLAGS += -DDEBUG
    CFLAGS_TEST += -DDEBUG
    MESSAGE = "Building in Debug Mode"
else
    MESSAGE = "Building in Release Mode"
endif

.PHONY: all build run clean format format-check bear test check

all: test

build:
	echo $(MESSAGE) && $(CC) $(CFLAGS_TEST) -o $(TEST_TARGET) $(TEST_SRC)

run:
	./$(TEST_TARGET)

clean:
	-rm -f $(TEST_TARGET)

format:
	find *.c *.h test/*.c | xargs clang-format -i --verbose

format-check:
	find *.c *.h test/*.c | xargs clang-format --dry-run --Werror --verbose

bear: # this is for creating the compile_commands.json file
	rm -f compile_commands.json && bear -- make build

check: format-check test

test: build run


