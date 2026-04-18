#include <stdio.h>
#include <time.h>
#include "hexagon.h"
#include "game_state.h"
#include "action.h"
#include "update.h"
#include "render.h"

void game_loop()
{
	GameState game_state;
	RenderState render_state;
	int key;
	Action action = get_user_action();
	update(&game_state, &render_state, action);
	render(&game_state, &render_state);
}

int main()
{
	game_loop();
	return 0;
}
