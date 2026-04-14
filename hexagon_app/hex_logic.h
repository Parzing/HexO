#ifndef HEX_LOGIC_H
#define HEX_LOGIC_H

#include "common.h"

void render_background();
int renderable(GameState *state, Position* pos);
void inject_value(Hexagon* hex);
void fill_hexagons(GameState *state);
void render_hex(GameState *state, Position* pos);
void render_center(GameState *state, Hexagon *curr);
void load_coordinates(Position* origin, int direction, Position *pos);
Hexagon* get_hexagon(GameState *state, Position* pos);
int detect_player_won(GameState *state, Position *pos);

#endif