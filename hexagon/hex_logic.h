#ifndef HEX_LOGIC_H
#define HEX_LOGIC_H

#include "common.h"

void render_background();
int renderable(GameState *state, struct Hexagon *hex);
void inject_value(struct Hexagon* hex);
void fill_hexagons(GameState *state);
void render_hex(GameState *state, struct Hexagon *curr);
void render_center(GameState *state, struct Hexagon *curr);
Hexagon* get_neighbor(struct Hexagon* hex, int direction);
void load_coordinates(struct Hexagon* hex, int direction, int *x, int *y);
Hexagon* get_hexagon(GameState *state, int x, int y);

#endif