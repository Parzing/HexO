#include <stdlib.h>

#include "game_engine.h"
#include "game_const.h"
#include "terminal.h"
#include "error.h"

void push_to(HexagonList **head, Hexagon *hex) {
    HexagonList *new_node = malloc(sizeof(HexagonList));
    new_node->hex = hex;
    new_node->next = *head;
    *head = new_node;
}

void update_hex_lists(ClientState *state, Hexagon *hex){
	if (hex->value == X) {
		push_to(&state->xHexList, hex);
	} else { // hex->value == O we hope 
		push_to(&state->oHexList, hex);
	} 
	// TODO: handle invalid hex case?
}

void update_cursor(ClientState *state){
	*(state->old_pos) = *(state->curr_pos);
	// load coordinates into current position based on old hexagon & direction
	load_coordinates(state->old_pos, state->key, state->curr_pos);
		
	if(!renderable(state, state->curr_pos)) {
		load_coordinates(state->anchor, state->key, state->anchor);
		state->values_changed = 1;
		state->background_changed = 1;
	}
	state->key = KEY_DEFAULT;
}

Hexagon* get_hexagon(ClientState *state, Position *pos){
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

int renderable(ClientState *state, Position *pos){
	int rel_x = pos->x - state->anchor->x;
    int rel_y = pos->y - state->anchor->y;

	int render_x = 2*rel_x + 1;
    int render_y = 4*rel_y - 2*rel_x + 2;

	if (render_x < 0 || render_x > terminal_x-1) return 0;
	if (render_y < 0 || render_y > terminal_y) return 0;

	return 1;
}

void load_coordinates(Position *origin, int direction, Position *dest){
	dest->x = origin->x;
	dest->y = origin->y;

	switch (direction) {
		case KEY_UP_L: 
			dest->x--;
			dest->y--;
			break;
		case KEY_UP_R: 
			dest->x--;
			break;
		case KEY_DOWN_L: 
			dest->x++;
			break;
		case KEY_DOWN_R:
			dest->x++; 
			dest->y++;
			break;
		case KEY_LEFT:
			dest->y--;
			break;
		case KEY_RIGHT:
			dest->y++;
			break;
	}
}

void cleanup(ClientState *state){
	free(state->server_hex->pos);
	free(state->server_hex);

	free(state->curr_pos);
	free(state->old_pos);
	free(state->anchor);

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
}
