# my C utils lib

- `mrs_strings.h`: string manipulation library
- `mrt_test.h`: mini test library 
- `mrl_logger.h`: logging utilities 
- `mrd_debug.h`: memory allocation debugging info. use LD_PRELOAD to enable logging eg:


#### mrd_debug example build
```Makefile
MEMORY_DEBUGGER_TARGET = mrd_debug.so
MEMORY_DEBUGGER_SRC = mrd_debug.c mrl_logger.c
MEMORY_DEBUGGER_LOG_LEVEL = MRD_LOG_LEVEL_ALL

DEBUG_ENABLED ?= 1
ifeq ($(DEBUG_ENABLED), 1)
    DEBUG_LD_PRELOAD = LD_PRELOAD=./$(MEMORY_DEBUGGER_TARGET)
    C_FLAGS += $(C_FLAGS_DEBUG)
endif

.PHONY: build run build-debugger-preload

all: build build-debugger-preload run

build:
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)

run:
	$(DEBUG_LD_PRELOAD) ./$(TEST_TARGET)

build-debugger-preload:
	$(CC) -D MRD_LOG_LEVEL_ALL -shared -fPIC -o $(MEMORY_DEBUGGER_TARGET) $(MEMORY_DEBUGGER_SRC)
```
note: `mrd_debug.h` should not be included in you main build silly


### TODO
- [ ] add left rigth comparison for failed tests 
- [ ] malloc calloc realloc free wrappers for debugging memory leaks 
- [ ] log to defined out 
- [ ] dynamic array lib 
TODODOODODO keep track of total alloced regions. if some not free, have some debug output.
eg: this PTR (id) with this DATA that was allocated HERE, was not freed
- [x] debug maintain only active allocations. eg: freed and not realloced to something not freed

i hope you like UB!


![https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif](https://static.wikia.nocookie.net/akagi/images/6/61/Shigeru_washizu_match.gif)
