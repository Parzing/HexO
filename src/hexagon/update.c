void update(GameState* state) {
	state->old_pos = state->curr_pos;

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
