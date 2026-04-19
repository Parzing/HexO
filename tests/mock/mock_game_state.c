#include <stddef.h>

#include "../src/game_state.h"
#include "../src/common.h"

void init_game(GameState *state) {
	state->moves_played = 1;
	state->player 		= X;
	state->winner 		= _;
	state->hexList 		= NULL;
}