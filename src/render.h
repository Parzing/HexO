#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include "hexagon.h"
#include "game_state.h"

typedef struct {
	Position curr_pos;
	Position old_pos;
	Position anchor;
	int terminal_x;
	int terminal_y;
	bool background_changed;
	bool values_changed;
} RenderState;

/*
 * Return whether hexagon in the given position is on-screen
 */
bool renderable(RenderState *render_state, Position pos);

/*
 * Render the game state to the screen, with anchor position
 * and terminal info from render_state.
 */
void render(GameState *state, RenderState *render_state);

#endif
