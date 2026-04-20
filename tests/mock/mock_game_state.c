#include <stddef.h>

#include "game_state.h"
#include "common.h"

void init_game(GameState *state) {
	state->moves_played = 1;
	state->player 		= X;
	state->winner 		= _;
	state->hexList 		= NULL;
}