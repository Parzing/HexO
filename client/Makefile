CC = gcc
# Added -D_DEFAULT_SOURCE for nanosleep and -g for debugging
CFLAGS = -Wall -Wextra -std=gnu11 -D_POSIX_C_SOURCE=199309L -D_DEFAULT_SOURCE -g -I ../common
LDFLAGS = 

# List of all your .c files
SRCS = client.c client_logic.c game_engine.c hex_display.c terminal.c \
		../common/hex_codex.c ../common/message.c ../common/socket.c ../common/logs.c


# This automatically creates a list of .o files from the .c files
OBJS = $(SRCS:.c=.o)

TARGET = client

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean