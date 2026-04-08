#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>

#include "hex_logic.h"
#include "common.h"
#include "terminal.h"

#define FRAME_NS 16666667
#define LEVEL ("level_test.txt")

int exit_loop = 0;
int terminal_x = DEFAULT_TERMINAL_X;
int terminal_y = DEFAULT_TERMINAL_Y;

int background_changed = 1;
int values_changed = 1;

void cleanup(GameState* state) {
	HexagonList *curr = state->oHexList;
	while (curr != NULL) {
		HexagonList *next = curr->next;
		free(curr->hex->pos);
		free(curr->hex);
		free(curr);
		curr = next;
	}
	

	curr = state->xHexList;
	while (curr != NULL) {
		HexagonList *next = curr->next;
		free(curr->hex->pos);
		free(curr->hex);
		free(curr);
		curr = next;
	}

	free(state->curr_pos);
	free(state->old_pos);
	free(state->anchor);
}

int player_out_of_bounds(GameState *state) {
	return !renderable(state, state->curr_pos);
}

void reset_player(GameState *state) {
	*state->curr_pos = *state->anchor;
}

int read_key(char* buf, int i) {
	switch (buf[i]) {
		case UPL: 		return KEY_UP_L;
		case UPR: 		return KEY_UP_R;
		case DOWNL: 	return KEY_DOWN_L;
		case DOWNR: 	return KEY_DOWN_R;
		case LEFT: 		return KEY_LEFT;
		case RIGHT: 	return KEY_RIGHT;
		case SPACE: 	return KEY_PLACE;
		case ENTER: 	return KEY_PLACE;
		default: 		return KEY_DEFAULT;
	}
	return 0;
}

void read_input(GameState* state) {
	char buf[64];
	int n = read(STDIN_FILENO, buf, sizeof(buf));
	int key;
	for(int i = 0; i < n; i++){
		key = read_key(buf, i);
		if (key == KEY_DEFAULT) {
			continue;
		}
		state->key = key;
		break;
	}
	
}

void render(GameState *state) {
	if (background_changed){
		render_background();
		// render current hexagon
		SET_CURR_COLOR;
		render_hex(state, state->curr_pos);
		render_center(state, get_hexagon(state, state->curr_pos));
		background_changed = 0;
	}
	if (values_changed){
		fill_hexagons(state);
		values_changed = 0;
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

void push_to(HexagonList **head, Hexagon *hex) {
    HexagonList *new_node = malloc(sizeof(HexagonList));
    new_node->hex = hex;
    new_node->next = *head;
    *head = new_node;
}

Hexagon* ensure_hexagon(GameState *state, Position* pos) {
	Hexagon *hex = get_hexagon(state, pos);
	if (hex != NULL) {
		return hex;
	}

	hex = malloc(sizeof(Hexagon));
	hex->value = _;
	hex->pos = malloc(sizeof(Position));
	*(hex->pos) = *pos;


	return hex;
}

void save_winner(GameState *state) {
	state->winner = state->player;
}
void update(GameState* state) {
	*state->old_pos = *state->curr_pos;

	if(state->key == KEY_DEFAULT) {
		return;
	}
	if(state->key == KEY_PLACE) {
		Hexagon* hex = ensure_hexagon(state, state->curr_pos);
		if(hex->value != _){
			return;
		}
		hex->value = state->player;
		if (hex->value == X) {
			push_to(&state->xHexList, hex);
		} else {
			push_to(&state->oHexList, hex);
		}

		if (detect_player_won(state, state->curr_pos)){
			save_winner(state);
			exit_loop = 1;
		}

		state->moves_played++;
		if(state->moves_played >= 2) {
			state->moves_played = 0;
			state->player = (state->player == X) ? O : X;
		}
		
		values_changed = 1;
		state->key = KEY_DEFAULT;
		return;
	}

	// load coordinates into current position based on old hexagon & direction
	load_coordinates(state->old_pos, state->key, state->curr_pos);
		
	if(!renderable(state, state->curr_pos)) {
		load_coordinates(state->anchor, state->key, state->anchor);
		values_changed = 1;
		background_changed = 1;
	}
	state->key = KEY_DEFAULT;
}


void load_level(GameState* state) {
	state->xHexList	= NULL;
	state->oHexList = NULL;
	int lattice_center_x = (terminal_x/2-1)/2;
	int lattice_center_y = (terminal_y / 2 + 2 * lattice_center_x - 2) / 4;

	state->anchor->x = 0;
	state->anchor->y = 0;
	
	*state->old_pos = (Position) {
		.x = lattice_center_x,
		.y = lattice_center_y
	};
	
	*state->curr_pos = (Position) {
		.x = lattice_center_x,
		.y = lattice_center_y
	};
	render(state);
}


int main() {
	configure_terminal();
	configure_parameters();
	signal(SIGINT, signal_handler);

	struct timespec start = {};
	struct timespec end = {};
	struct timespec sleep = {};

	CLEAR_SCREEN;
	GameState state = {
		.key = KEY_DEFAULT,
		.player = X,
		.moves_played = 1
	};

	state.curr_pos = malloc(sizeof(Position));
	state.old_pos = malloc(sizeof(Position));
	state.anchor = malloc(sizeof(Position));

	load_level(&state);

	while(!exit_loop) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		
		read_input(&state);
		update(&state);
		render(&state);

		if (terminal_resized()) {
			configure_parameters();
			CLEAR_SCREEN;
			background_changed = 1;
			values_changed = 1;
			if(player_out_of_bounds(&state)){
				reset_player(&state);
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &end);

		long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
		long remaining_ns = FRAME_NS - elapsed_ns;

		if (remaining_ns > 0) {
			sleep.tv_nsec = remaining_ns;
			nanosleep(&sleep, NULL);
    	}
	}

	cleanup(&state);
}