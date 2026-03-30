#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <string.h>

#include "hex_logic.h"
#include "common.h"
#include "terminal.h"

#define FRAME_NS 16666667
#define LEVEL ("level_test.txt")

int exit_loop = 0;
int terminal_x = DEFAULT_TERMINAL_X;
int terminal_y = DEFAULT_TERMINAL_Y;


//TODO: DELETE LATER
// used for loading level
int hex_x = 10;
int hex_y = 20;

int background_changed = 1;
int values_changed = 1;

void cleanup(GameState* state) {
	HexagonList *curr = state->list;
	while (curr != NULL) {
		HexagonList *next = curr->next;
		free(curr->hex);
		free(curr);
		curr = next;
	}

	curr = state->xHexList;
	while (curr != NULL) {
		HexagonList *next = curr->next;
		free(curr);
		curr = next;
	}
	curr = state->oHexList;
	while (curr != NULL) {
		HexagonList *next = curr->next;
		free(curr);
		curr = next;
	}

	state->list 	= NULL;
	state->oHexList = NULL;
	state->xHexList = NULL;
}

int player_out_of_bounds(GameState *state) {
	return !renderable(state, state->curr);
}

void reset_player(GameState *state) {
	state->curr = state->top_left;
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
		SET_CURR_COLOR;
		render_hex(state, state->curr);
		render_center(state, state->curr);
		background_changed = 0;
	}
	if (values_changed){
		fill_hexagons(state);
		values_changed = 0;
	}
	if (state->curr != state->old){
		SET_LATTICE_COLOR;
		render_hex(state, state->old);
		SET_CURR_COLOR;
		render_hex(state, state->curr);
	}
	fflush(stdout);
}

void push_to(HexagonList **head, Hexagon *hex) {
    HexagonList *new_node = malloc(sizeof(HexagonList));
    new_node->hex = hex;
    new_node->next = *head;
    *head = new_node;
}

Hexagon* ensure_hexagon(GameState *state, int x, int y) {
	Hexagon *hex = get_hexagon(state, x, y);
	if (hex != NULL) {
		return hex;
	}

	hex = malloc(sizeof(Hexagon));
	*hex = (Hexagon) {
		.value = _,
		.x_pos = x,
		.y_pos = y,
		.upL 	= NULL,
		.upR 	= NULL,
		.downL 	= NULL,
		.downR 	= NULL,
		.left 	= NULL,
		.right 	= NULL
	};
	
	push_to(&state->list, hex);
	return hex;
}

Hexagon* generate_hex(GameState *state, Hexagon *origin, int direction) {
	int x, y;
	load_coordinates(origin, direction, &x, &y);
	Hexagon *hex = ensure_hexagon(state, x, y);

	int directions[] = {KEY_UP_L, KEY_UP_R, KEY_DOWN_L, KEY_DOWN_R, KEY_LEFT, KEY_RIGHT};

	// link generated hexagon with hexagons in every direction (if exists)
	Hexagon *temp;

	for(int i = 0; i < 6; i++) {
		load_coordinates(hex, directions[i], &x, &y);
		temp = get_hexagon(state, x, y);
		if(temp == NULL) {
			continue;
		}
		switch (directions[i]){
			case KEY_UP_L:		hex->upL	= temp; temp->downR = hex; break;
			case KEY_UP_R:		hex->upR	= temp; temp->downL = hex; break;
			case KEY_DOWN_L:	hex->downL	= temp; temp->upR	= hex; break;
			case KEY_DOWN_R:	hex->downR	= temp; temp->upL	= hex; break;
			case KEY_LEFT:		hex->left	= temp; temp->right	= hex; break;
			case KEY_RIGHT:		hex->right	= temp; temp->left	= hex; break;
		}
	}
	return hex;
}

void save_winner(GameState *state) {
	state->winner = state->curr->value;
}

void update(GameState* state) {
	state->old = state->curr;
	if(state->key == KEY_DEFAULT) {
		return;
	}
	else if(state->key == KEY_PLACE) {
		if(state->curr->value != _){
			return;
		}
		state->curr->value = state->player;
		state->moves_played++;
		if(state->moves_played >= 2) {
			state->moves_played = 0;
			state->player = (state->player == X) ? O : X;
		}
		if (detect_player_won(state->curr)){
			save_winner(state);
			exit_loop = 1;
		}
		values_changed = 1;
		state->key = KEY_DEFAULT;
		return;
	}


	Hexagon* new = get_neighbor(state->curr, state->key);
	if (new == NULL) {
		new = generate_hex(state, state->curr, state->key);	
	}
	
	if(!renderable(state, new)) {
		Hexagon* new_top_left = get_neighbor(state->top_left, state->key);
		if (new_top_left == NULL) {
			new_top_left = generate_hex(state, state->top_left, state->key);
		}
		state->top_left = new_top_left;
		values_changed = 1;
		background_changed = 1;
	}

	state->curr = new;
	state->key = KEY_DEFAULT;
}


void load_level(GameState* state) {
	state->list 		= NULL;
	state->xHexList 	= NULL;
	state->oHexList 	= NULL;
	int lattice_center_x = (terminal_x/2-1)/2;
	int lattice_center_y = (terminal_y / 2 + 2 * lattice_center_x - 2) / 4;

	state->top_left = ensure_hexagon(state, 0, 0);
	state->curr = ensure_hexagon(state, lattice_center_x, lattice_center_y);
	state->old = state->curr;

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