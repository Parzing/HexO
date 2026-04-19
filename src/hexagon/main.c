#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "hexagon.h"
#include "game_state.h"
#include "action.h"
#include "update.h"
#include "render.h"
#include "terminal.h"

void cleanup(GameState *game_state, __attribute__((unused)) RenderState *render_state) {
	// note: render_state does not need to have anything cleaned up atm
	HexagonList *curr = game_state->hexList;
	while (curr != NULL) {
		HexagonList *next = curr->next;
		free(curr->hex);
		free(curr);
		curr = next;
	}
}


void play_game()
{
	GameState game_state;
	RenderState render_state;
	init_game(&game_state);
	init_render_state(&render_state);
	while (game_state.winner == _) {
		if (interrupt_received) {
			break;
		}
		Action action = get_user_action();
		update(&game_state, &render_state, action);
		render(&game_state, &render_state);
	}
	cleanup(&game_state, &render_state);
	reset_terminal();
}

int main()
{
	play_game();
	return 0;
}
