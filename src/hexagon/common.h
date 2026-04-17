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

enum Key {
	KEY_DEFAULT,
	KEY_UP_L,
	KEY_UP_R,
	KEY_DOWN_L,
	KEY_DOWN_R,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_PLACE,
	KEY_REMOVE
};


#define X ('X')
#define O ('O')
#define _ (' ')

extern int terminal_x;
extern int terminal_y;
extern int exit_loop;
extern int background_changed;
extern int values_changed;


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

// x is <->
// y is bottom left to top right
typedef struct {
	int key;
	int moves_played;
	char winner;
	char player;

	Position curr_pos;
	Position old_pos;
	Position anchor;

	HexagonList *xHexList;
	HexagonList *oHexList;
} GameState;


typedef 
#endif