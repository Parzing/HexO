CC = gcc

# --- Port Configuration ---
# Default port for the project.
PORT ?= 61674

# --- Directory Definitions ---
TEST_DIR = testing
TEST_COMMON = $(TEST_DIR)/common
TEST_CLIENT = $(TEST_DIR)/client
TEST_SERVER = $(TEST_DIR)/server
TEST_HEX    = $(TEST_DIR)/hexagon

# --- Compiler Flags ---
# Includes project directories and the new testing subfolders.
CFLAGS = -Wall -Wextra -std=gnu11 -D_POSIX_C_SOURCE=199309L -D_DEFAULT_SOURCE -g \
         -I./common -I./client_app -I./server_app -I./hexagon_app \
         -I./$(TEST_COMMON) -I./$(TEST_CLIENT) -I./$(TEST_SERVER) -I./$(TEST_HEX) \
         -DPORT=$(PORT)
LDFLAGS = 

# --- Source Definitions ---
COMMON_SRCS = common/hex_codex.c common/message.c common/socket.c common/logs.c

CLIENT_SRCS = client_app/client.c client_app/client_logic.c client_app/game_engine.c \
              client_app/hex_display.c client_app/terminal.c $(COMMON_SRCS)

SERVER_SRCS = server_app/server.c server_app/server_logic.c server_app/network_handler.c \
              server_app/game_engine.c $(COMMON_SRCS)

HEX_SRCS = hexagon_app/hexagon.c hexagon_app/hex_logic.c hexagon_app/terminal.c

# --- Test Source Definitions ---
TEST_COMMON_SRCS = $(TEST_COMMON)/test_common.c $(COMMON_SRCS)
TEST_CLIENT_SRCS = $(TEST_CLIENT)/test_client.c $(CLIENT_SRCS)
TEST_SERVER_SRCS = $(TEST_SERVER)/test_server.c $(SERVER_SRCS)
TEST_HEX_SRCS    = $(TEST_HEX)/test_hexagon.c $(HEX_SRCS)

# --- Object Definitions ---
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o) 
HEX_OBJS    = $(HEX_SRCS:.c=.o)

TEST_COMMON_OBJS = $(TEST_COMMON_SRCS:.c=.o)
TEST_CLIENT_OBJS = $(TEST_CLIENT_SRCS:.c=.o)
TEST_SERVER_OBJS = $(TEST_SERVER_SRCS:.c=.o)
TEST_HEX_OBJS    = $(TEST_HEX_SRCS:.c=.o)

# --- Binary Targets (with directory paths) ---
CLIENT_TARGET = client
SERVER_TARGET = server
HEX_TARGET    = hexagon

# Adding paths to the test binaries
TEST_COMMON_BIN = $(TEST_COMMON)/test_common
TEST_CLIENT_BIN = $(TEST_CLIENT)/test_client
TEST_SERVER_BIN = $(TEST_SERVER)/test_server
TEST_HEX_BIN    = $(TEST_HEX)/test_hexagon

ALL_TESTS = $(TEST_COMMON_BIN) $(TEST_CLIENT_BIN) $(TEST_SERVER_BIN) $(TEST_HEX_BIN)

# Default target [cite: 2]
all: $(CLIENT_TARGET) $(SERVER_TARGET) $(HEX_TARGET)

# Main Application Rules [cite: 2]
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(HEX_TARGET): $(HEX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# --- Testing Rules ---
tests: $(ALL_TESTS)

$(TEST_COMMON_BIN): $(TEST_COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_CLIENT_BIN): $(TEST_CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_SERVER_BIN): $(TEST_SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_HEX_BIN): $(TEST_HEX_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Helper to run all tests
run_tests: tests
	@echo "Running Common Tests..." && ./$(TEST_COMMON_BIN)
	@echo "Running Client Tests..." && ./$(TEST_CLIENT_BIN)
	@echo "Running Server Tests..." && ./$(TEST_SERVER_BIN)
	@echo "Running Hexagon Tests..." && ./$(TEST_HEX_BIN)

# Standard rule for compiling .c to .o [cite: 2]
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup all binaries and object files [cite: 2]
clean:
	rm -f $(CLIENT_TARGET) $(SERVER_TARGET) $(HEX_TARGET) $(ALL_TESTS)
	find . -name "*.o" -type f -delete

.PHONY: all clean tests run_tests