#include "hex_codex.h"
#include "game_const.h"

#include "game_engine.h"
#include "server_data.h"
#include "logs.h"

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

Hexagon* get_hexagon(ServerState *state, Position* pos) {
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

// detects if the hexagon is not in the list of hexagons we already have
int valid_move(ServerState *state, Hexagon *hex){
	if(hex->value != state->player) {
		return 0;
	}

	Hexagon *new_hex = get_hexagon(state, hex->pos);
	if (new_hex == NULL) {
		return 1;
	}

	return 0;
}

void push_to(HexagonList **head, Hexagon *hex) {
    HexagonList *new_node = malloc(sizeof(HexagonList));
    new_node->hex = hex;
    new_node->next = *head;
    *head = new_node;
}

int count_dir(ServerState *state, struct Hexagon *hex, int direction) {
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

void compute_winner(ServerState* state, Position* pos) {

	Hexagon *hex = get_hexagon(state, pos);

	if (count_dir(state, hex, KEY_UP_L) + count_dir(state, hex, KEY_DOWN_R) + 1 >= 6) {
		state->winner = hex->value;
		return;
	}

	if (count_dir(state, hex, KEY_UP_R) + count_dir(state, hex, KEY_DOWN_L) + 1 >= 6) {
		state->winner = hex->value;
		return;
	}

	if (count_dir(state, hex, KEY_LEFT) + count_dir(state, hex, KEY_RIGHT) + 1 >= 6) {
		state->winner = hex->value;
		return;
	}
}

void process_move(ServerState *state, Hexagon *hex){
	state->curr_move = hex;
	if (hex->value != X && hex->value != O) {
		char *buffer;
		buffer = encode(hex);
		log_message("Error processing hexagon: ");
		log_message(buffer);
		log_message("\n");
		free(buffer);
	}
	state->num_moves++;
	if (state->num_moves >= 2) {
		state->num_moves = 0;
		state->player = (state->player == X) ? O : X;
	}

	if(hex->value == X) {
		push_to(&(state->xHexList), hex);
	} else {
		push_to(&(state->oHexList), hex);
	}

	compute_winner(state, hex->pos);
}

int has_winner(ServerState *state){
	if(state->winner == X || state->winner == O){
		return 1;
	}
	return 0;
}

void cleanup_game(ServerState *state) {
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

	// if for some reason the current move is not in the lists above
	state->curr_move = NULL;
}
