#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "client_logic.h"

void update_hex_lists(ClientState *state, Hexagon *hex);
void update_cursor(ClientState *state);
void reset_cursor(ClientState *state);

Hexagon* get_hexagon(ClientState *state, Position *pos);
int renderable(ClientState *state, Position *pos);
void load_coordinates(Position *origin, int direction, Position *dest);

void cleanup(ClientState *state);

#endif