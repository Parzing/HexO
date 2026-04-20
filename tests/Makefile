# --- Compiler and Flags ---
CC       := gcc
# -I../src/hexagon includes that folder so the makefile can see it
CFLAGS   := -Wall -Wextra -g -I. -I../src/hexagon

BUILD_DIR 		:= build
MOCK_DIR 		:= mock
HEX_TEST_DIR 	:= hexagon
HEX_SRC_DIR 	:= ../src/hexagon

TEST_SRC := tester.c
TEST_OBJ := $(BUILD_DIR)/tester.o
TEST_H	 := tester.h

# all .c files from src/hexagon, minus main.c
HEX_NAMES := $(filter-out main, $(patsubst $(HEX_SRC_DIR)/%.c, %, $(wildcard $(HEX_SRC_DIR)/*.c)))
HEX_MOCK_SRCS := $(patsubst %, $(MOCK_DIR)/mock_%.c, $(HEX_NAMES))
HEX_TARGETS := $(patsubst %, $(BUILD_DIR)/test_%, $(HEX_NAMES))



all: $(HEX_TARGETS)

# ensures build is an actual folder
$(BUILD_DIR): 
	@mkdir -p $(BUILD_DIR)

# so for every name (ex: action) we take the test file, the source file, and tester
# and combine that with every mock file in mock, minus the one we're on rn
# "throw everything in there" compilation lmao

# helpful shortcuts, so u can do make action instead of make build/test_action
$(HEX_NAMES): %: $(BUILD_DIR)/test_%

# this one is just test_action, etc, instead of just action
$(patsubst %, test_%, $(HEX_NAMES)): test_%: $(BUILD_DIR)/test_%

$(BUILD_DIR)/test_%: $(HEX_TEST_DIR)/test_%.c $(HEX_SRC_DIR)/%.c tester.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ \
	$(filter-out $(MOCK_DIR)/mock_$*.c, $(HEX_MOCK_SRCS)) \
	-o $@

# Cleanup
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean $(HEX_NAMES) $(patsubst %, test_%, $(HEX_NAMES))