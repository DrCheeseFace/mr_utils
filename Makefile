CC = gcc

CSTANDARD = c99

CFLAGS_TEST = -Wpointer-arith -Wcast-align \
         -Wstrict-prototypes -Wwrite-strings -Waggregate-return \
         -Wswitch-default -Wswitch-enum -Wunreachable-code \
	 -Wunused-parameter -Wuninitialized -Winit-self \
 	 -Wbad-function-cast -Wcast-align\
	 -Wformat=2 -Wlogical-op -Wmissing-include-dirs \
         -Wredundant-decls -Wsequence-point -Wshadow \
	 -Wswitch -Wundef -Wunused-but-set-parameter \
	 -Wcast-qual  -Wfloat-equal -Wnested-externs \
	 -g \
	 -fsanitize=address \
	 -Wpedantic  -pedantic-errors \
	 
CFLAGS = -Wall -Wextra -Werror \
	 -std=$(CSTANDARD) \
	 -O2

TEST_TARGET = test.out
TEST_SRC =  test/*.c mrl_logger.c mrs_strings.c mrt_test.c

MEMORY_DEBUGGER_TARGET = mrd_debug.so
MEMORY_DEBUGGER_SRC = mrd_debug.c mrl_logger.c

DEBUG_ENABLED ?= 1
ifeq ($(DEBUG_ENABLED), 1)
    LD_PRELOAD = LD_PRELOAD=./$(MEMORY_DEBUGGER_TARGET)
    C_FLAGS += $(C_FLAGS_TEST)
endif


.PHONY: all build run clean format format-check bear test check build-debugger-preload

all: test

build:
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)

run:
	$(LD_PRELOAD) ./$(TEST_TARGET)

clean:
	-rm -f $(TEST_TARGET)

format:
	find *.c *.h test/*.c | xargs clang-format -i --verbose

format-check:
	find *.c *.h test/*.c | xargs clang-format --dry-run --Werror --verbose

bear: # this is for creating the compile_commands.json file
	rm -f compile_commands.json && bear -- make build

check: format-check test

test: build-debugger-preload build run

build-debugger-preload:
	$(CC) -shared -fPIC -o $(MEMORY_DEBUGGER_TARGET) $(MEMORY_DEBUGGER_SRC)
