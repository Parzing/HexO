#ifndef GAME_CONS_H
#define GAME_CONS_H

#include <stdio.h>
#include <stdlib.h>

// default port.
#define DEFAULT_PORT 61674

// ANSI esc sequences
#define CLEAR_SCREEN (printf("\e[2J"))
#define HIDE_CURSOR (printf("\e[?25l"))
#define SHOW_CURSOR (printf("\e[?25h"))
#define CURSOR_TO(x, y) (printf("\e[%d;%dH", (x) + 1, (y) + 1))
#define RESET_COLOR (printf("\e[m"))
#define DISABLE_WRAPPING (printf("\e[?7l"))
#define ENABLE_WRAPPING (printf("\e[?7h"))

#define SET_LATTICE_COLOR (printf("\033[38;5;244m"))
#define SET_CURR_COLOR (printf("\033[38;5;254m"))

#define BOLD (printf("\e[1m"))
#define X_COLOR (printf("\x1B[0;31m"))
#define O_COLOR (printf("\x1B[0;34m"))

// game keys for movement
#define UPL ('q')
#define UPR ('e')
#define DOWNL ('z')
#define DOWNR ('c')
#define LEFT ('a')
#define RIGHT ('d')
#define SPACE (' ')
#define ENTER ('\n')

// for internal processing (key_remove unused)
#define KEY_DEFAULT 0
#define KEY_UP_L	1
#define KEY_UP_R	2
#define KEY_DOWN_L	3
#define KEY_DOWN_R	4
#define KEY_LEFT	5
#define KEY_RIGHT	6
#define KEY_PLACE	7
#define KEY_REMOVE	8

#define SERVER_FULL ("FULL")
#define X_WINS ("X wins!")
#define O_WINS ("O wins!")

#define X ('X')
#define O ('O')
#define _ (' ')

// structs for game logic
typedef struct Position{
	int x;
	int y;
} Position;

typedef struct Hexagon{
	char value;
	struct Position* pos;
} Hexagon;

typedef struct HexagonList {
	struct Hexagon *hex;
	struct HexagonList *next;
} HexagonList;

#endif