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
    CFLAGS     := -O0 -g -fno-omit-frame-pointer -rdynamic -DDEBUG -DMRD_DEBUG_BACKTRACE $(WARNINGS) $(INCLUDES)
else
    BUILD_TYPE := release
    CFLAGS     := -O2 $(WARNINGS) $(INCLUDES)
endif

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/$(BUILD_TYPE)

TARGET_TEST    = $(OBJ_DIR)/test.out
TARGET_SPACERS = $(OBJ_DIR)/spacers

SRC_COMMON     = src/mrd_debug.c \
	         src/mrl_logger.c \
	         src/mrs_strings.c \
	         src/mrt_test.c \
	         src/mrv_vectors.c \

SRC_TEST       = test/test.c

SRC_TOOLS      = tools/spacers.c

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
