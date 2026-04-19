# --- Compiler and Flags ---
CC       := gcc
# -I../src/hexagon includes that folder so the makefile can see it
CFLAGS   := -Wall -Wextra -g -I. -I../src/hexagon

BUILD_DIR 			:= build
MOCK_DIR 			:= mock
HEXAGON_TEST_DIR 	:= hexagon
HEXAGON_SRC_DIR 	:= ../src/hexagon

TEST_SRC := tester.c
TEST_OBJ := $(BUILD_DIR)/tester.o
TEST_H	 := tester.h

HEX_SRCS := $(filter-out $(HEXAGON_SRC_DIR)/main.c, $(wildcard $(HEXAGON_SRC_DIR)/*.c))
HEX_OBJS := $(patsubst $(HEXAGON_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(HEX_SRCS))


# gets all .c files in hexagon as hexagon/xxx.c
HEX_TEST_SRCS := $(wildcard $(HEXAGON_TEST_DIR)/*.c)
# turns them into targets as xxx.c (curr directory)
HEX_TARGETS  := $(patsubst $(HEXAGON_TEST_DIR)/%.c, $(BUILD_DIR)/%, $(HEX_TEST_SRCS))

all: $(HEX_TARGETS)

# need to make specific the mock and real for each test; theres prob a better way but idk it lmao
TEST_RENDER_OBJS = mock_game_state.o mock_terminal.o mock_hexagon.o
$(BUILD_DIR)/test_render: MOCK_OBJS = $(patsubst %, $(BUILD_DIR)/%, $(TEST_RENDER_OBJS))

TEST_UPDATE_OBJS = mock_game_state.o mock_terminal.o mock_render.o mock_action.o
$(BUILD_DIR)/test_update: MOCK_OBJS = $(patsubst %, $(BUILD_DIR)/%, $(TEST_UPDATE_OBJS))


# ensures build is an actual folder
$(BUILD_DIR): 
	@mkdir -p $(BUILD_DIR)

# makes test files
$(BUILD_DIR)/test_%: $(BUILD_DIR)/test_%.o $(TEST_OBJ) $(HEX_OBJS) $(MOCK_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# compile object files
$(BUILD_DIR)/%.o: $(HEXAGON_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# compile mock ,o files
$(BUILD_DIR)/mock_%.o: $(MOCK_DIR)/mock_%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ): $(TEST_SRC) $(TEST_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%.o: $(HEXAGON_TEST_DIR)/test_%.c $(TEST_SRC) $(TEST_H) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup
clean:
	rm -rf $(BUILD_DIR)
	rm $(HEX_TARGETS)

# so tester.o also gets deleted
.INTERMEDIATE: $(TEST_OBJ)
.PHONY: all clean