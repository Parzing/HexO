#include <stdbool.h>
#include <stdio.h>
#include "render.h"
#include "game_state.h"
#include "hexagon.h"
#include "common.h"
#include "terminal.h"

bool renderable(RenderState *render_state, Position pos) {
	render_state->terminal_x = pos.x;
	return true;
}

void render(GameState *state, RenderState *render_state) {
	state->winner = X;
	render_state->terminal_x = 100;
}

void init_render_state(RenderState *render_state) {
	render_state->terminal_x = 100;
}