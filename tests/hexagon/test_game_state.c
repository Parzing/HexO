#include <stdio.h>

#include "../../src/hexagon/game_state.h"
#include "../../src/hexagon/common.h"
#include "../tester.h"

bool test_init(void) {
	GameState state;
	init_game(&state);
	return (state.moves_played == 1 && 
			state.player == X &&
			state.winner == _ &&
			state.hexList == NULL);
}

int main(int argc, char **argv) {
	TestCase tests[] = {
		{test_init, "test_init", "Tests if the game state was correctly initialized"}
	};

	int test_num = sizeof(tests)/sizeof(TestCase);
	run_tests(tests, test_num, argc, argv);
}