#include <stdio.h>
#include "common.h"
#include "hex_logic.h"

const char tiling [4][4] = {
	{".+'+"},
	{"|   "},
	{"'+.+"},
	{"  | "}
};

void render_background() {
	CURSOR_TO(0,0);
	SET_LATTICE_COLOR;

	char row[terminal_y+1];
	row[terminal_y] = '\0';
	// four row types: top border, middle, bottom border, middle shifted
	int row_type = 0;

	for(int i = 0; i < terminal_x; i++){
		row_type = i%4;
		for(int j = 0; j < terminal_y; j++) {
			row[j] = tiling[row_type][j%4];
		}
		printf("%s", row);
		if (i != terminal_x - 1) {
			printf("\n");
		}
	}		
}

int renderable(GameState *state, struct Hexagon *hex) {
	int rel_x = hex->x_pos - state->top_left->x_pos;
    int rel_y = hex->y_pos - state->top_left->y_pos;

	int render_x = 2*rel_x + 1;
    int render_y = 4*rel_y - 2*rel_x + 2;

	if (render_x < 0 || render_x > terminal_x-1) return 0;
	if (render_y < 0 || render_y > terminal_y) return 0;


	return 1;
}

void inject_value(struct Hexagon* hex) {
	if (hex->value == X) {
		X_COLOR;
		printf("%c", X);
	} else if (hex->value == O) {
		O_COLOR;
		printf("%c",O);
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

	for(int dx = -1; dx <= 1; dx++) {
		for (int dy = -2; dy <= 2; dy++) {
			if (cursor_center_x + dx < 0 || cursor_center_x + dx > terminal_x-1 ||
				cursor_center_y + dy < 0 || cursor_center_y + dy > terminal_y){
				continue;
			}
			// we don't want to overwrite the shown value of this hexagon
			if(dx == dy && dy == 0) {
				continue;
			}
			CURSOR_TO(cursor_center_x + dx, cursor_center_y + dy);
			printf("%c", tiling[dx+1][(dy+2)%4]);
		}
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