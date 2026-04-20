#include <stddef.h>
#include "game_state.h"
#include "terminal.h"
#include "render.h"
#include "common.h"
#include "action.h"


/*
	Returns coordinates of a position that you would get by moving in direction from origin.
	If the action is not a direction, returns origin.
*/
Position load_coordinates (Position origin, Action direction) {
	Position pos;
	pos.x = origin.x;
	pos.y = origin.y;

	switch (direction) {
		case ACT_UP_L: 
			pos.x--;
			pos.y--;
			break;
		case ACT_UP_R: 
			pos.x--;
			break;
		case ACT_DOWN_L: 
			pos.x++;
			break;
		case ACT_DOWN_R:
			pos.x++; 
			pos.y++;
			break;
		case ACT_LEFT:
			pos.y--;
			break;
		case ACT_RIGHT:
			pos.y++;
			break;
		default:
			break;
	}
	return pos;
}

int count_dir(GameState *state, struct Hexagon *hex, Action direction) {
	Hexagon *curr = hex;
	Position new_pos;
	int count = 0;
	for(count = 0; count < 6; count++){
		new_pos = load_coordinates(curr->pos, direction);
		curr = get_hexagon(state->hexList, new_pos);
		if (curr == NULL || curr->value != hex->value){
			return count;
		}
	}
	return count;
}

int detect_player_won(GameState* state, Position pos) {

	Hexagon *hex = get_hexagon(state->hexList, pos);

	if (count_dir(state, hex, ACT_UP_L) + count_dir(state, hex, ACT_DOWN_R) + 1 >= 6) {
		return 1;
	}

	if (count_dir(state, hex, ACT_UP_R) + count_dir(state, hex, ACT_DOWN_L) + 1 >= 6) {
		return 1;
	}

		if (count_dir(state, hex, ACT_LEFT) + count_dir(state, hex, ACT_RIGHT) + 1 >= 6) {
		return 1;
	}
	return 0;
}

void update(GameState* state, RenderState *render, Action action) {
	render->old_pos = render->curr_pos;
	// load coordinates into current position based on old hexagon & direction.
	render->curr_pos = load_coordinates(render->old_pos, action);
		
	if(!renderable(render, render->curr_pos)) {
		render->anchor = load_coordinates(render->anchor, action);
		render->values_changed = true;
	}

	if(terminal_resized()){
		configure_parameters(render);
		render->background_changed = true;
		render->values_changed = true;
	}

	if(action == ACT_DEFAULT) {
		return;
	}
	if(action == ACT_PLACE) {
		Hexagon* hex = ensure_hexagon(state->hexList, render->curr_pos);
		if(hex->value != _){
			return;
		}
		hex->value = state->player;
		push_to(&state->hexList, hex);

		if (detect_player_won(state, render->curr_pos)){
			Hexagon *winner = get_hexagon(state->hexList, render->curr_pos);
			state->winner = winner->value;
		}

		state->moves_played++;
		if(state->moves_played >= 2) {
			state->moves_played = 0;
			state->player = (state->player == X) ? O : X;
		}
		
		render->values_changed = true;
		return;
	}




}
