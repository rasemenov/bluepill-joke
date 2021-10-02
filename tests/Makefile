TOP_DIR  := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR  := ../src
UNITY_DIR := runner
BUILD_DIR := build
INC_DIRS := $(shell find $(SRC_DIR) -maxdepth 1 -type d)
INC_DIRS += $(UNITY_DIR)
TEST_SRC  = $(shell find ./ -maxdepth 1 -name \*.c)
TEST_BIN := $(TEST_SRC:%.c=$(BUILD_DIR)/%)
INCLUDES := $(addprefix -I,$(INC_DIRS))

CC := $(shell which gcc)

.PHONY: test clean

$(TEST_BIN): $(TEST_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(TGT_CFLAGS) $(INCLUDES) runner/unity.c $< -o $@

test: $(TEST_BIN)
	@for bin in $(TEST_BIN); do \
		$$bin; \
	done

clean:
	rm -rf $(BUILD_DIR)
