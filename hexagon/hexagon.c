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

// how much of the lattice is shown on screen
int lattice_x = 10;
int lattice_y = 16;

//TODO: DELETE LATER
// used for loading level
int hex_x = 10;
int hex_y = 20;

int background_changed = 1;
int values_changed = 1;

char foo[10][20] = {
			 {  X,X,O,X,_,X,_,O,O,_,X,_,X,_,O,O,_,O,O,X},
			{  O,X,_,_,O,_,_,O,O,O,O,O,O,X,X,X,_,O,X,_},
		   {_,  _,O,X,_,_,O,O,O,_,O,_,X,X,X,O,X,X,O,X},
		  {_,  O,O,O,O,_,O,X,_,O,X,X,X,O,_,X,X,O,_,_},
		 {O,X,  O,_,_,O,X,_,X,X,X,O,O,_,X,_,O,O,_,_},
		{_,O,  _,X,X,_,_,_,_,X,O,O,O,_,X,_,_,_,O,O},
	   {O,O,O,  O,O,O,O,O,_,X,O,O,X,_,_,_,X,_,_,_},
	  {O,O,_,  X,X,O,X,O,X,X,_,O,O,_,X,O,X,O,_,O},
	 {O,O,X,_,  _,X,_,O,O,O,X,O,X,_,_,O,_,O,O,O},
	{O,_,_,O,  _,O,X,X,_,_,_,_,_,_,O,X,_,_,_,O}
};

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

int read_key(char* buf, int i) {
	switch (buf[i]) {
		case UPL: 	return KEY_UP_L;
		case UPR: 	return KEY_UP_R;
		case DOWNL: return KEY_DOWN_L;
		case DOWNR: return KEY_DOWN_R;
		case LEFT: 	return KEY_LEFT;
		case RIGHT: return KEY_RIGHT;
		case SPACE: return KEY_PLACE;
		case ENTER: return KEY_PLACE;
		default: 	return KEY_DEFAULT;
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

void push_to(HexagonList **head, Hexagon *h) {
    HexagonList *new_node = malloc(sizeof(HexagonList));
    new_node->hex = h;
    new_node->next = *head;
    *head = new_node;
}


Hexagon* generate_hex(GameState *state, Hexagon *origin, int direction) {
	int x, y;
	load_coordinates(origin, direction, &x, &y);

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

void update(GameState* state) {
	state->old = state->curr;
	if(state->key == KEY_DEFAULT) {
		return;
	}

	if(state->key == KEY_PLACE) {
		state->curr->value = state->player;
		state->player = (state->player == X) ? O : X;
		values_changed = 1;
	}
	else if(state->key == KEY_REMOVE) {
		state->curr->value = _;
		values_changed = 1;
	} else {
		Hexagon* new = get_neighbor(state->curr, state->key);
		if (new == NULL) {
			new = generate_hex(state, state->curr, state->key);	
		}
		state->curr = new;
	}
	state->key = KEY_DEFAULT;
}



void load_level(GameState* state) {

	Hexagon* stateArr[hex_x][hex_y];

	state->list 		= NULL;
	state->xHexList 	= NULL;
	state->oHexList 	= NULL;

	// set up all the hexagons with appropriate values/positions
	for(int i = 0; i < hex_x; i++){
		for (int j = 0; j < hex_y; j++) {
			Hexagon *temp = malloc(sizeof(Hexagon));
			temp->value = foo[i][j];
			temp->x_pos = i;
			temp->y_pos = j;
			stateArr[i][j] = temp;
		}
	}

	// loop thru all hexagons in arr and attach appropriately
	for (int i = 0; i < hex_x; i++) {
		for (int j = 0; j < hex_y; j++) {
			Hexagon *h = stateArr[i][j];

			h->left  = (j > 0)			?	stateArr[i][j - 1]	: NULL;
			h->right = (j < hex_y - 1)	?	stateArr[i][j + 1]	: NULL;

			h->upL   = (i > 0 && j > 0)	?	stateArr[i - 1][j - 1]	: NULL;
			h->upR   = (i > 0)			?	stateArr[i - 1][j]		: NULL;

			h->downL = (i < hex_x - 1)					?	stateArr[i + 1][j]		: NULL;
			h->downR = (i < hex_x - 1 && j < hex_y - 1) ?	stateArr[i + 1][j + 1]	: NULL;
		}
	}

	 for (int i = 0; i < hex_x; i++) {
        for (int j = 0; j < hex_y; j++) {
            Hexagon *temp = stateArr[i][j];
            
            // Add to master list
            push_to(&state->list, temp);
            
            // Add to specific color lists
            if (temp->value == X) {
                push_to(&state->xHexList, temp);
            } else if (temp->value == O) {
                push_to(&state->oHexList, temp);
            }
        }
    }

	state->top_left = stateArr[0][0];
	state->curr = stateArr[hex_x/2][hex_y/2];
	state->old = state->curr;

	render(state);
}


int main() {
	configure_terminal();

	signal(SIGINT, signal_handler);

	struct timespec start = {};
	struct timespec end = {};
	struct timespec sleep = {};
	struct winsize window;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)

	CLEAR_SCREEN;
	GameState state = {
		.key = 0,
		.player = X
	};



	load_level(&state);


	while(!exit_loop) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		
		read_input(&state);
		update(&state);
		render(&state);

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