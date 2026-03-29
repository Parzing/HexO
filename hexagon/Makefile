CC = gcc
CFLAGS = -Wall -Wextra -O2
OBJ = hexagon.o terminal.o hex_logic.o

all: hexagon

hexagon: $(OBJ)
	$(CC) $(OBJ) -o hexagon

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o hexagon