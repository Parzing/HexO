#include <stddef.h>
#include <stdio.h>

#include "game_state.h"
#include "terminal.h"
#include "render.h"
#include "common.h"
#include "action.h"

#define KEY_UPL ('q')
#define KEY_UPR ('e')
#define KEY_DOWNL ('z')
#define KEY_DOWNR ('c')
#define KEY_LEFT ('a')
#define KEY_RIGHT ('d')
#define KEY_SPACE (' ')
#define KEY_ENTER ('\n')

typedef struct Position {
	int x;
	int y;
} Position;

typedef struct Hexagon {
	char value;
	struct Position pos;
} Hexagon;

typedef struct HexagonList {
	struct Hexagon *hex;
	struct HexagonList *next;
} HexagonList;

typedef enum {
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

typedef struct {
	Position curr_pos;
	Position old_pos;
	Position anchor;
	int terminal_x;
	int terminal_y;
	bool background_changed;
	bool values_changed;
} RenderState;


void update(GameState* state, RenderState *render, Action action) {
	printf("Game state: "
			"	Moves played:	%d\n"
			"	Player:			%c\n"
			"	Winner:			%c\n",
			state->moves_played, state->player, state->winner);
	if(state != NULL) {
		printf(
			"	HexList[0]: 	(%c, %d, %d)\n\n", 
			state->hexList->hex->value, state->hexList->hex->pos.x, state->hexList->hex->pos.y);
	} else {
		printf("	HexList[0]:		NULL");
	}
	
	if(render != NULL) {
		printf(
			"RenderState:\n"
			"	curr_pos:		(%d, %d)\n"
			"	old_pos:		(%d, %d)\n"
			"	anchor:			(%d, %d)\n"
			"	terminal:		(%d, %d)\n"
			"	bg_changed:		%d\n"
			"	values_changed:	%d\n\n",
			render->curr_pos.x, render->curr_pos.y,
			render->old_pos.x, render->old_pos.y,
			render->anchor.x, render->anchor.y,
			render->terminal_x, render->terminal_y,
			render->background_changed,
			render->values_changed
		);
	} else {
		printf("RenderState: NULL\n\n");
	}


	char *action_str = "";

	switch(action) {
		case ACT_DEFAULT:	action_str = "ACT_DEFAULT"; break;
		case ACT_UP_L:		action_str = "ACT_UP_L"; break;
		case ACT_UP_R:		action_str = "ACT_UP_R"; break;
		case ACT_DOWN_L:	action_str = "ACT_DOWN_L"; break;
		case ACT_DOWN_R:	action_str = "ACT_DOWN_R"; break;
		case ACT_LEFT:		action_str = "ACT_LEFT"; break;
		case ACT_RIGHT:		action_str = "ACT_RIGHT"; break;
		case ACT_PLACE:		action_str = "ACT_PLACE"; break;
		case ACT_REMOVE:	action_str = "ACT_REMOVE"; break;
	}

	printf("Action:				%s", action_str);
}
