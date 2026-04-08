CC = gcc
# Added -D_DEFAULT_SOURCE for nanosleep and -g for debugging
CFLAGS = -Wall -Wextra -std=gnu11 -D_POSIX_C_SOURCE=199309L -D_DEFAULT_SOURCE -g \
         -I./common -I./client_app -I./server_app -I./hexagon_app
LDFLAGS = 

# --- Source Definitions ---
COMMON_SRCS = common/hex_codex.c common/message.c common/socket.c common/logs.c

# Client sources
CLIENT_SRCS = client_app/client.c client_app/client_logic.c client_app/game_engine.c \
              client_app/hex_display.c client_app/terminal.c $(COMMON_SRCS)

# Server sources
SERVER_SRCS = server_app/server.c server_app/server_logic.c server_app/network_handler.c \
              server_app/game_engine.c $(COMMON_SRCS)

# Hexagon sources for local game running
HEX_SRCS = hexagon_app/hexagon.c hexagon_app/hex_logic.c hexagon_app/terminal.c

# --- Object Definitions ---

CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
HEX_OBJS = $(HEX_SRCS:.c=.o)

# All objects for cleaning
ALL_OBJS = $(CLIENT_OBJS) $(SERVER_OBJS) $(HEX_OBJS)

# --- Targets ---

CLIENT_TARGET = client
SERVER_TARGET = server
HEX_TARGET = hexagon

all: $(CLIENT_TARGET) $(SERVER_TARGET) $(HEX_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJS) $(LDFLAGS)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJS) $(LDFLAGS)

$(HEX_TARGET): $(HEX_OBJS)
	$(CC) $(CFLAGS) -o $(HEX_TARGET) $(HEX_OBJS) $(LDFLAGS)

# Standard rule for compiling .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(ALL_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET) $(HEX_TARGET)

.PHONY: all clean