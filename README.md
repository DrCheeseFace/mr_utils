# my C utils lib

- `mrs_strings.h`: string manipulation library
- `mrt_test.h`: mini test library 
- `mrl_logger.h`: logging utilities 
- `mrd_debug.h`: memory allocation debugging info. use LD_PRELOAD to enable logging eg:
- `mrm_misc.h`: useful defines and cafebabe!

#### mrd_debug example build
```Makefile
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
	 # -fsanitize=address \
	 
CFLAGS = -Wall -Wextra -Werror \
	 -std=$(CSTANDARD) \
	 -O2

TEST_TARGET = test.out
TEST_SRC =  test/*.c *.c

MEMORY_DEBUGGER_LOG_LEVEL = MRD_LOG_LEVEL_NORMAL
# MEMORY_DEBUGGER_LOG_LEVEL = MRD_LOG_LEVEL_ERR_ONLY
# MEMORY_DEBUGGER_LOG_LEVEL = MRD_LOG_LEVEL_ALL

.PHONY: all build run clean format format-check bear test check build-debugger-preload debug build-debug 

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
	$(CC) $(CFLAGS_DEBUG) -D $(MEMORY_DEBUGGER_LOG_LEVEL) -D DEBUG -o $(TEST_TARGET) $(TEST_SRC)
```
note: `mrd_debug.h` should not be included in you main build silly


### TODO
- [ ] add left rigth comparison for failed tests 
- [ ] malloc calloc realloc free wrappers for debugging memory leaks eg: this PTR (id) with this DATA that was allocated HERE, was not freed
- [ ] log to defined out 
- [ ] dynamic array lib 
- [ ] doc generator tool 
- [ ] useful runtime error message. something with coredump file abort()
- [x] debug maintain only active allocations. eg: freed and not realloced to something not freed


i hope you like UB!


![https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif](https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif)
