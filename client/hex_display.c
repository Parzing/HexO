#include <stdio.h>
#include "common.h"
#include "terminal.h"
#include "hex_display.h"
#include "game_engine.h"

// Lattice tiling
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

void inject_value(struct Hexagon* hex) {
	if (hex->value == X) {
		X_COLOR;
		printf("%c", X);
	} else if (hex->value == O) {
		O_COLOR;
		printf("%c",O);
	}
}

void fill_hexagons(ClientState *state) {
	
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

void render_hex(ClientState *state, Position* pos) {
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

void render_center(ClientState *state, Hexagon *curr) {
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

void render(ClientState *state) {
	if (state->background_changed){
		render_background();
		// render current hexagon
		SET_CURR_COLOR;
		render_hex(state, state->curr_pos);
		render_center(state, get_hexagon(state, state->curr_pos));
		state->background_changed = 0;
	}
	if (state->values_changed){
		fill_hexagons(state);
		state->values_changed = 0;
	}
	if (state->curr_pos->x != state->old_pos->x || 
		state->curr_pos->y != state->old_pos->y){
		SET_LATTICE_COLOR;
		render_hex(state, state->old_pos);
		SET_CURR_COLOR;
		render_hex(state, state->curr_pos);
	}
	fflush(stdout);
}