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

typedef enum Action {
	ACT_DEFAULT,
	ACT_UP_L,
	ACT_UP_R,
	ACT_DOWN_L,
	ACT_DOWN_R,
	ACT_LEFT,
	ACT_RIGHT,
	ACT_PLACE,
	ACT_REMOVE
} Action;


#define X ('X')
#define O ('O')
#define _ (' ')


extern int exit_loop;
extern int background_changed;
extern int values_changed;




typedef struct Position{
	int x;
	int y;
} Position;

typedef struct Hexagon{
	char value;
	struct Position pos;
} Hexagon;

typedef struct HexagonList {
	struct Hexagon *hex;
	struct HexagonList *next;
} HexagonList;

typedef struct {
	int values_changed;
	int background_changed;

	int terminal_x;
	int terminal_y;

	Position curr_pos;
	Position old_pos;
	Position anchor;

} RenderState;

typedef struct {
	int moves_played;
	char winner;
	char player;

	HexagonList *hexList;
} GameState;



#endif