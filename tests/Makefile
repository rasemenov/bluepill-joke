TOP_DIR  := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR  := ../src
UNITY_DIR := runner
CMOCK_DIR := cmock
BUILD_DIR := build
INC_DIRS := $(shell find $(SRC_DIR) -maxdepth 1 -type d)
INC_DIRS += $(UNITY_DIR)
INC_DIRS += $(CMOCK_DIR)
TEST_SRC  = $(shell find ./ -maxdepth 1 -name \*.c)
SRCFILES  = $(UNITY_DIR)/unity.c $(CMOCK_DIR)/cmock.c $(SRC_DIR)/sys/serial.c
SRCFILES += $(shell find mocks -name \*.c)
TEST_BIN := $(TEST_SRC:%.c=$(BUILD_DIR)/%)
INCLUDES := $(addprefix -I,$(INC_DIRS))
DEFINES  := -DUNIT_TESTS

CC := $(shell which gcc)

.PHONY: test clean

$(TEST_BIN): $(TEST_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(TGT_CFLAGS) $(DEFINES) $(INCLUDES) $(SRCFILES) $< -o $@

test: $(TEST_BIN)
	@for bin in $(TEST_BIN); do \
		$$bin; \
	done

clean:
	rm -rf $(BUILD_DIR)
