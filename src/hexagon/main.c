#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "hexagon.h"
#include "game_state.h"
#include "action.h"
#include "update.h"
#include "render.h"
#include "terminal.h"

void init_game(GameState *state) {
	struct timespec start = {};
	struct timespec end = {};
	struct timespec sleep = {};

	state->player = X;
	state->moves_played = 1;

	state->hexList = NULL;
}

void init_render_state(RenderState *render_state)
{
	configure_terminal();
	configure_parameters(render_state);
	signal(SIGINT, signal_handler);

	render_state->terminal_x = DEFAULT_TERMINAL_X;
	render_state->terminal_y = DEFAULT_TERMINAL_Y;
	int lattice_center_x = (render_state->terminal_x/2-1)/2;
	int lattice_center_y = (render_state->terminal_y / 2 + 2 * lattice_center_x - 2) / 4;

	render_state->anchor.x = 0;
	render_state->anchor.y = 0;
	
	render_state->old_pos = (Position) {
		.x = lattice_center_x,
		.y = lattice_center_y
	};
	
	render_state->curr_pos = (Position) {
		.x = lattice_center_x,
		.y = lattice_center_y
	};

	CLEAR_SCREEN();
}

void play_game()
{
	GameState game_state;
	RenderState render_state;
	init_game(&game_state);
	init_render_state(&render_state);
	while (true) {
		Action action = get_user_action();
		update(&game_state, &render_state, action);
		render(&game_state, &render_state);
	}
}

int main()
{
	play_game();
	return 0;
}
