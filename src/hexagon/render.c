#include <stdbool.h>
#include <stdio.h>
#include "render.h"
#include "game_state.h"
#include "hexagon.h"
#include "common.h"
#include "terminal.h"

const char tiling [4][4] = {
	{".+'+"},
	{"|   "},
	{"'+.+"},
	{"  | "}
};

void inject_value(struct Hexagon* hex) {
	if (hex->value == X) {
		X_COLOR();
		printf("%c", X);
	} else if (hex->value == O) {
		O_COLOR();
		printf("%c", O);
	}
}

static void render_hex(RenderState *render_state, Position pos) {
	int rel_x = pos.x - render_state->anchor.x;
	int rel_y = pos.y - render_state->anchor.y;
	// center = rel_x * 2 + 1, rel_y*4+2 - rel_x*2
	int cursor_center_x = rel_x * 2 + 1;
	int cursor_center_y = rel_y*4+2 - rel_x*2;

	for(int dx = -1; dx <= 1; dx++) {
		for (int dy = -2; dy <= 2; dy++) {
			if (cursor_center_x + dx < 0 || cursor_center_x + dx > render_state->terminal_x-1 ||
				cursor_center_y + dy < 0 || cursor_center_y + dy > render_state->terminal_y-1){
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

static void fill_hexagons(GameState *state, RenderState *render_state) {
	int rel_x;
	int rel_y;
	int render_x;
	int render_y;

	HexagonList *curr = state->hexList;
	while(curr != NULL) {
		if(renderable(render_state, curr->hex->pos)){
			rel_x = curr->hex->pos.x - render_state->anchor.x;
			rel_y = curr->hex->pos.y - render_state->anchor.y;

			render_x = 2*rel_x + 1;
			render_y = 4*rel_y - 2*rel_x + 2;
			CURSOR_TO(render_x, render_y);
			inject_value(curr->hex);
		}
		curr = curr->next;
	}
}

static void render_center(GameState *state, RenderState *render_state, Hexagon *curr) {
	if(curr == NULL) {
		return;
	}
	int rel_x = curr->pos.x - render_state->anchor.x;
	int rel_y = curr->pos.y - render_state->anchor.y;
	int cursor_center_x = rel_x * 2 + 1;
	int cursor_center_y = rel_y*4+2 - rel_x*2;
	CURSOR_TO(cursor_center_x, cursor_center_y);
	inject_value(get_hexagon(state->hexList, curr->pos));
}


static void render_background(RenderState *render_state) {
	CLEAR_SCREEN();
	CURSOR_TO(0,0);
	SET_LATTICE_COLOR();

	char row[render_state->terminal_y+1];
	row[render_state->terminal_y] = '\0';
	// four row types: top border, middle, bottom border, middle shifted
	int row_type = 0;

	for(int i = 0; i < render_state->terminal_x; i++){
		row_type = i%4;
		for(int j = 0; j < render_state->terminal_y; j++) {
			row[j] = tiling[row_type][j%4];
		}
		printf("%s", row);
		if (i != render_state->terminal_x - 1) {
			printf("\n");
		}
	}		
}

bool renderable(RenderState *render_state, Position pos) {
	int rel_x = pos.x - render_state->anchor.x;
	int rel_y = pos.y - render_state->anchor.y;

	int render_x = 2*rel_x + 1;
	int render_y = 4*rel_y - 2*rel_x + 2;
	if (render_x < 0 || render_x > render_state->terminal_x-1) return false;
	if (render_y < 0 || render_y > render_state->terminal_y) return false;

	return true;
}

void render(GameState *state, RenderState *render_state) {
	if (render_state->background_changed){
		render_background(render_state);
		// render current hexagon
		switch (state->player) {
			case X: X_COLOR(); break;
			case O: O_COLOR(); break;
		}
		render_hex(render_state, render_state->curr_pos);
		render_center(state, render_state, get_hexagon(state->hexList, render_state->curr_pos));
		render_state->background_changed = false;
	}
	if (render_state->values_changed){
		fill_hexagons(state, render_state);
		switch (state->player) {
			case X: X_COLOR(); break;
			case O: O_COLOR(); break;
		}
		render_hex(render_state, render_state->curr_pos);
		render_state->values_changed = false;
	}
	if (render_state->curr_pos.x != render_state->old_pos.x || 
		render_state->curr_pos.y != render_state->old_pos.y){
		SET_LATTICE_COLOR();
		render_hex(render_state, render_state->old_pos);
		switch (state->player) {
			case X: X_COLOR(); break;
			case O: O_COLOR(); break;
		}
		render_hex(render_state, render_state->curr_pos);
	}
	fflush(stdout);
}

void init_render_state(RenderState *render_state) {
	configure_terminal();
	configure_parameters(render_state);
	signal(SIGINT, signal_handler);

	int lattice_center_x = (render_state->terminal_x/2-1)/2;
	int lattice_center_y = (render_state->terminal_y / 2 + 2 * lattice_center_x - 2) / 4;

	render_state->background_changed = true;
	render_state->values_changed = true;

	render_state->anchor.x = 0;
	render_state->anchor.y = 0;
	
	render_state->old_pos = (Position) {
		.x = lattice_center_x,
		.y = lattice_center_y
	};
	
	render_state->curr_pos = (Position) {
		.x = lattice_center_x,
		.y = lattice_center_y
	};
}