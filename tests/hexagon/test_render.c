#include "../../src/hexagon/render.h"
#include "../tester.h"

bool test_not_renderable(void) {
	Position pos = {0, 0};
	RenderState render_state = {
		.anchor = (Position) {1,1},
		.terminal_x = 100,
		.terminal_y = 100
	};
	return !renderable(&render_state, pos);
}

bool test_renderable(void) {
	Position pos = {1, 1};
	RenderState render_state = {
		.anchor = (Position) {0, 0},
		.terminal_x = 100,
		.terminal_y = 100
	};
	return renderable(&render_state, pos);
}

bool test_init(void) {
	RenderState render_state;
	init_render_state(&render_state);
	if (render_state.curr_pos.x != 24	||
		render_state.curr_pos.y != 24	||
		render_state.old_pos.x 	!= 24	||
		render_state.old_pos.y 	!= 24	||
		render_state.anchor.x 	!= 0	||
		render_state.anchor.y 	!= 0	||
		render_state.terminal_x != 100	||
		render_state.terminal_y != 100	||
		render_state.background_changed != true ||
		render_state.values_changed != true) {
		return false;
	}
	return true;
}

int main(int argc, char **argv) {
	TestCase tests[] = {
		{test_not_renderable, "test_not_renderable", "Tests if a position is not renderable"},
		{test_renderable, "test_renderable", "Tests if a position is renderable"},
		{test_init, "test_init", "Tests initialization"}
	};

	int test_num = sizeof(tests)/sizeof(TestCase);
	run_tests(tests, test_num, argc, argv);
}