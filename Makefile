CC = gcc

CSTANDARD = c99

CFLAGS_DEBUG = -Wall -Wextra -Werror \
               -Wpointer-arith -Wcast-align \
               -Wstrict-prototypes -Wwrite-strings -Waggregate-return \
               -Wswitch-default -Wswitch-enum -Wunreachable-code \
               -Wunused-parameter -Wuninitialized -Winit-self \
               -Wbad-function-cast -Wcast-align\
               -Wformat=2 -Wlogical-op -Wmissing-include-dirs \
               -Wredundant-decls -Wsequence-point -Wshadow \
               -Wswitch -Wundef -Wunused-but-set-parameter \
               -Wcast-qual  -Wfloat-equal -Wnested-externs \
               -O0 -g \
               -Wpedantic  -pedantic-errors \
               -DDEBUG
	 # -fsanitize=address \
	 
CFLAGS = -Wall -Wextra -Werror \
	 -std=$(CSTANDARD) \
	 -O2

TEST_TARGET = test.out
TEST_SRC =  test/*.c *.c

.PHONY: all build run clean format format-check bear test check debug build-debug 

all: test

test: build run

build:
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)

run:
	./$(TEST_TARGET)

clean:
	-rm -f $(TEST_TARGET)

format:
	find *.c *.h test/* | xargs clang-format -i --verbose

format-check:
	find *.c *.h test/* | xargs clang-format --dry-run --Werror --verbose

bear: # this is for creating the compile_commands.json file
	rm -f compile_commands.json && bear -- make build-debug

check: format-check build-debug run

debug:  build-debug run

build-debug:
	$(CC) $(CFLAGS_DEBUG) -o $(TEST_TARGET) $(TEST_SRC)
