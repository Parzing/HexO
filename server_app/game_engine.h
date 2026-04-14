#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "game_const.h"
#include "server_data.h"

int valid_move(ServerState *state, Hexagon *hex);
void process_move(ServerState *state, Hexagon *hex);
int has_winner(ServerState *state);
void cleanup_game(ServerState *state);

#endif