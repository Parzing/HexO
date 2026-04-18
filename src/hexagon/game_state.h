#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "hexagon.h"

typedef struct {
	int moves_played;
	char player;
	char winner;
	HexagonList *hexList;
} GameState;

void init_game(GameState *state);

#endif
