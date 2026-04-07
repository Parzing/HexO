CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -D_POSIX_C_SOURCE=199309L -D_DEFAULT_SOURCE -g -I . -I ../common

# Core server-side logic files
SERVER_LOGIC_SRCS = server_logic.c game_engine.c network_handler.c

# Common utility files used by almost everything
COMMON_SRCS = ../common/hex_codex.c ../common/message.c ../common/socket.c ../common/logs.c

# Minimal requirements for the test files
TEST_SRCS = ../common/message.c ../common/socket.c

# Final Source lists
SRCS = server.c $(SERVER_LOGIC_SRCS) $(COMMON_SRCS)
TEST_SERVER_SRCS = test_server.c $(TEST_SRCS)
TEST_CLIENT_SRCS = test_client.c $(TEST_SRCS)

# Targets
TARGET = server
TEST_SERVER = test_server
TEST_CLIENT = test_client

all: $(TARGET) $(TEST_SERVER) $(TEST_CLIENT)

$(TARGET): $(SRCS:.c=.o)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS:.c=.o)

$(TEST_SERVER): $(TEST_SERVER_SRCS:.c=.o)
	$(CC) $(CFLAGS) -o $(TEST_SERVER) $(TEST_SERVER_SRCS:.c=.o)

$(TEST_CLIENT): $(TEST_CLIENT_SRCS:.c=.o)
	$(CC) $(CFLAGS) -o $(TEST_CLIENT) $(TEST_CLIENT_SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o ../common/*.o $(TARGET) $(TEST_SERVER) $(TEST_CLIENT)

.PHONY: all clean