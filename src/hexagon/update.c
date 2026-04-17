int renderable(RenderState *render, Position pos) {
	int rel_x = pos.x - render->curr_pos.x;
    int rel_y = pos.y - render->curr_pos.y;

	int render_x = 2*rel_x + 1;
    int render_y = 4*rel_y - 2*rel_x + 2;

	if (render_x < 0 || render_x > render->terminal_x-1) return 0;
	if (render_y < 0 || render_y > render->terminal_y) return 0;

	return 1;
}

void load_coordinates(Position* origin, Action action, Position *pos){
	pos->x = origin->x;
	pos->y = origin->y;

	switch (direction) {
		case ACT_UP_L: 
			pos->x--;
			pos->y--;
			break;
		case ACT_UP_R: 
			pos->x--;
			break;
		case ACT_DOWN_L: 
			pos->x++;
			break;
		case ACT_DOWN_R:
			pos->x++; 
			pos->y++;
			break;
		case ACT_LEFT:
			pos->y--;
			break;
		case ACT_RIGHT:
			pos->y++;
			break;
	}
}

Hexagon* get_hexagon(GameState *state, Position pos) {
	HexagonList *curr = state->hexList;
	while(curr != NULL) {
		if (curr->hex->pos.x == pos.x && curr->hex->pos.y == pos.y) {
			break;
		}
		curr = curr->next;
	}
	return curr;
}

Hexagon* ensure_hexagon(GameState *state, Position pos) {
	Hexagon *hex = get_hexagon(state, pos);
	if (hex != NULL) {
		return hex;
	}

	hex = malloc(sizeof(Hexagon));
	hex->value = _;
	hex->pos = pos;

	return hex;
}

int count_dir(GameState *state, struct Hexagon *hex, Action action) {
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

int detect_player_won(GameState* state, Position pos) {

	Hexagon *hex = get_hexagon(state, pos);

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

void push_to(HexagonList **head, Hexagon *hex) {
    HexagonList *new_node = malloc(sizeof(HexagonList));
    new_node->hex = hex;
    new_node->next = *head;
    *head = new_node;
}

void update(GameState* state, RenderState *render, Action action) {
	state->old_pos = state->curr_pos;

	if(action == ACT_DEFAULT) {
		return;
	}
	if(action == ACT_PLACE) {
		Hexagon* hex = ensure_hexagon(state, render->curr_pos);
		if(hex->value != _){
			return;
		}
		hex->value = state->player;
		push_to(&state->hexList, hex);

		if (detect_player_won(state, render->curr_pos)){
			state->winner = get_hexagon(state, render->curr_pos);
		}

		state->moves_played++;
		if(state->moves_played >= 2) {
			state->moves_played = 0;
			state->player = (state->player == X) ? O : X;
		}
		
		render->values_changed = 1;
		return;
	}

	// load coordinates into current position based on old hexagon & direction
	load_coordinates(&render->old_pos, action, &render->curr_pos);
		
	if(!renderable(render)) {
		load_coordinates(state->anchor, state->key, state->anchor);
		render->values_changed = 1;
		render->background_changed = 1;
	}
}
