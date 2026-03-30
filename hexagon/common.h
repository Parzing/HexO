#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

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

#define UPL ('q')
#define UPR ('e')
#define DOWNL ('z')
#define DOWNR ('c')
#define LEFT ('a')
#define RIGHT ('d')
#define SPACE (' ')
#define ENTER ('\n')

#define KEY_DEFAULT 0
#define KEY_UP_L	1
#define KEY_UP_R	2
#define KEY_DOWN_L	3
#define KEY_DOWN_R	4
#define KEY_LEFT	5
#define KEY_RIGHT	6
#define KEY_PLACE	7
#define KEY_REMOVE	8

#define X ('X')
#define O ('O')
#define _ (' ')

extern int terminal_x;
extern int terminal_y;
extern int exit_loop;
extern int background_changed;
extern int values_changed;


typedef struct Hexagon{
	char value;
	int x_pos;
	int y_pos;
	struct Hexagon *upL;
	struct Hexagon *upR;
	struct Hexagon *downL;
	struct Hexagon *downR;
	struct Hexagon *left;
	struct Hexagon *right;
} Hexagon;

typedef struct HexagonList {
	struct Hexagon *hex;
	struct HexagonList *next;
} HexagonList;

// x is <->
// y is bottom left to top right
typedef struct {
	int key;
	int moves_played;
	char winner;
	char player;
	struct Hexagon *curr;
	struct Hexagon *old;
	struct Hexagon *top_left;
	struct HexagonList *list;
	struct HexagonList *xHexList;
	struct HexagonList *oHexList;
} GameState;

#endif