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

int renderable(GameState *state, Position* pos) {
	int rel_x = pos->x - state->anchor->x;
    int rel_y = pos->y - state->anchor->y;

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

	HexagonList *curr = state->xHexList;
	while(curr != NULL) {
		if(renderable(state, curr->hex->pos)){
			rel_x = curr->hex->pos->x - state->anchor->x;
			rel_y = curr->hex->pos->y - state->anchor->y;

			render_x = 2*rel_x + 1;
			render_y = 4*rel_y - 2*rel_x + 2;
			CURSOR_TO(render_x, render_y);
			inject_value(curr->hex);
		}
		curr = curr->next;
	}
	curr = state->oHexList;
	while(curr != NULL) {
		if(renderable(state, curr->hex->pos)){
			rel_x = curr->hex->pos->x - state->anchor->x;
			rel_y = curr->hex->pos->y - state->anchor->y;

			render_x = 2*rel_x + 1;
			render_y = 4*rel_y - 2*rel_x + 2;
			CURSOR_TO(render_x, render_y);
			inject_value(curr->hex);
		}
		curr = curr->next;
	}

}

void render_hex(GameState *state, Position* pos) {
	int rel_x = pos->x - state->anchor->x;
	int rel_y = pos->y - state->anchor->y;
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

void render_center(GameState *state, Hexagon *curr) {
	if(curr == NULL) {
		return;
	}
	int rel_x = curr->pos->x - state->anchor->x;
	int rel_y = curr->pos->y - state->anchor->y;
	int cursor_center_x = rel_x * 2 + 1;
	int cursor_center_y = rel_y*4+2 - rel_x*2;
	CURSOR_TO(cursor_center_x, cursor_center_y);
	inject_value(get_hexagon(state, curr->pos));
}

void load_coordinates(Position* origin, int direction, Position *pos){
	
	pos->x = origin->x;
	pos->y = origin->y;

	switch (direction) {
		case KEY_UP_L: 
			pos->x--;
			pos->y--;
			break;
		case KEY_UP_R: 
			pos->x--;
			break;
		case KEY_DOWN_L: 
			pos->x++;
			break;
		case KEY_DOWN_R:
			pos->x++; 
			pos->y++;
			break;
		case KEY_LEFT:
			pos->y--;
			break;
		case KEY_RIGHT:
			pos->y++;
			break;
	}
}

Hexagon* get_hexagon(GameState *state, Position* pos) {
	HexagonList *curr = state->xHexList;
	while(curr != NULL) {
		if (curr->hex->pos->x == pos->x && curr->hex->pos->y == pos->y) {
			return curr->hex;
		}
		curr = curr->next;
	}

	curr = state->oHexList;
	while(curr != NULL) {
		if (curr->hex->pos->x == pos->x && curr->hex->pos->y == pos->y) {
			return curr->hex;
		}
		curr = curr->next;
	}

	return NULL;
}

int count_dir(GameState *state, struct Hexagon *hex, int direction) {
	Hexagon *curr = hex;
	Position new_pos;
	int count = 0;
	for(count = 0; count < 6; count++){
		load_coordinates(curr->pos, direction, &new_pos);
		curr = get_hexagon(state, &new_pos);
		if (curr == NULL || curr->value != hex->value){
			return count;
		}
	}
	return count;
}

int detect_player_won(GameState* state, Position* pos) {

	Hexagon *hex = get_hexagon(state, pos);

	if (count_dir(state, hex, KEY_UP_L) + count_dir(state, hex, KEY_DOWN_R) + 1 >= 6) {
		return 1;
	}

	if (count_dir(state, hex, KEY_UP_R) + count_dir(state, hex, KEY_DOWN_L) + 1 >= 6) {
		return 1;
	}

		if (count_dir(state, hex, KEY_LEFT) + count_dir(state, hex, KEY_RIGHT) + 1 >= 6) {
		return 1;
	}
	return 0;
}