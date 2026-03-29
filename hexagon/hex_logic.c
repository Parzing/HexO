#include <stdio.h>
#include "common.h"
#include "hex_logic.h"

void render_background() {
	CURSOR_TO(0,0);
	SET_LATTICE_COLOR;
	for(int row = 0; row < lattice_x; row++) {
		if((row % 2) == 0) {
			// print top border
			for(int col = 0; col < lattice_y; col++) {
				printf(".+'+");
			}
			printf(".\n");
			// print internal
			for(int col = (row/2); col < (lattice_y+row/2); col++) {
				printf("|   ");
			}
			printf("|\n");
		} else {
			// print top border
			for(int col = 0; col < lattice_y; col++) {
				printf("'+.+");
			}
			printf("'\n");
			// print internal
			for(int col = (row-1)/2; col < (lattice_y+(row-1)/2); col++) {
				printf("  | ");
			}
			printf(" \n");
		}
	}
	for(int col = 0; col < lattice_y; col++) {
		if (lattice_x % 2 == 0){
			printf(".+'+");
		} else {
			printf("'+.+");
		}
	}
	if (lattice_x % 2 == 0) {
		printf(".\n");
	} else {
		printf("'\n");
	}
}

int renderable(GameState *state, struct Hexagon *hex) {
	int rel_x = hex->x_pos - state->top_left->x_pos;
    int rel_y = hex->y_pos - state->top_left->y_pos;

	// top/bottom border
	if (rel_x < 0 || rel_x >= lattice_x) {
		return 0;
	}

	// left/right border
	if(rel_y*2 - rel_x + 1< 0) {
		return 0;
	}
	if (2*rel_y - rel_x + 1 - 2*lattice_y  - (rel_x % 2) >= 0 ) {
		return 0;
	}

	return 1;
}

void inject_value(struct Hexagon* hex) {
	if (hex->value == X) {
		printf("%s%c", X_COLOR, X);
	} else if (hex->value == O) {
		printf("%s%c", O_COLOR, O);
	}
}

void fill_hexagons(GameState *state) {
	
	int rel_x;
	int rel_y;
	int render_x;
	int render_y;

	HexagonList *curr = state->list;
	while(curr != NULL) {
		if(renderable(state, curr->hex)){
			rel_x = curr->hex->x_pos - state->top_left->x_pos;
			rel_y = curr->hex->y_pos - state->top_left->y_pos;

			render_x = 2*rel_x + 1;
			render_y = 4*rel_y - 2*rel_x + 2;
			CURSOR_TO(render_x, render_y);
			inject_value(curr->hex);
		}
		curr = curr->next;
	}
}

void render_hex(GameState *state, struct Hexagon *curr) {
	int rel_x = curr->x_pos - state->top_left->x_pos;
	int rel_y = curr->y_pos - state->top_left->y_pos;
	// center = rel_x * 2 + 1, rel_y*4+2 - rel_x*2
	int cursor_center_x = rel_x * 2 + 1;
	int cursor_center_y = rel_y*4+2 - rel_x*2;

	// !left edge?
	if(!(rel_y*2 + 1 == rel_x)) {
		CURSOR_TO(cursor_center_x-1, cursor_center_y-2);
		printf(".+");
		CURSOR_TO(cursor_center_x, cursor_center_y-2);
		printf("| ");
		CURSOR_TO(cursor_center_x+1, cursor_center_y-2);
		printf("'+");
	}
	// center
	CURSOR_TO(cursor_center_x-1, cursor_center_y);
	printf("'");
	CURSOR_TO(cursor_center_x+1, cursor_center_y);
	printf(".");

	//!right edge?

	if(!((rel_x % 2) == 1 && rel_x == ((rel_y-lattice_y-1)*2-1))) {
		CURSOR_TO(cursor_center_x-1, cursor_center_y+1);
		printf("+.");
		CURSOR_TO(cursor_center_x, cursor_center_y+1);
		printf(" |");
		CURSOR_TO(cursor_center_x+1, cursor_center_y+1);
		printf("+'");
	}
}

void render_center(GameState *state, struct Hexagon *curr) {
	int rel_x = curr->x_pos - state->top_left->x_pos;
	int rel_y = curr->y_pos - state->top_left->y_pos;
	int cursor_center_x = rel_x * 2 + 1;
	int cursor_center_y = rel_y*4+2 - rel_x*2;
	CURSOR_TO(cursor_center_x, cursor_center_y);
	inject_value(curr);
}

Hexagon* get_neighbor(struct Hexagon* hex, int direction) {
	if (!hex) {
		return NULL;
	}
	switch (direction) {
		case KEY_UP_L: 		return hex->upL;
		case KEY_UP_R: 		return hex->upR;
		case KEY_DOWN_L: 	return hex->downL;
		case KEY_DOWN_R: 	return hex->downR;
		case KEY_LEFT: 		return hex->left;
		case KEY_RIGHT: 	return hex->right;
		default: return NULL;
	}
}

void load_coordinates(struct Hexagon* hex, int direction, int *x, int *y){
	*x = hex->x_pos;
	*y = hex->y_pos;

	switch (direction) {
		case KEY_UP_L: 
			(*x)--;
			(*y)--;
			break;
		case KEY_UP_R: 
			(*x)--;
			break;
		case KEY_DOWN_L: 
			(*x)++;
			break;
		case KEY_DOWN_R:
			(*x)++; 
			(*y)++;
			break;
		case KEY_LEFT:
			(*y)--;
			break;
		case KEY_RIGHT:
			(*y)++;
			break;
	}
}

Hexagon* get_hexagon(GameState *state, int x, int y) {
	HexagonList *curr = state->list;
	while(curr != NULL) {
		if (curr->hex->x_pos == x && curr->hex->y_pos == y) {
			return curr->hex;
		}
		curr = curr->next;
	}
	return NULL;
}