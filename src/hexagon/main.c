void init_application(GameState *state) {
	configure_terminal();
	configure_parameters();
	signal(SIGINT, signal_handler);

	struct timespec start = {};
	struct timespec end = {};
	struct timespec sleep = {};

	CLEAR_SCREEN;
	state->key = KEY_DEFAULT;
	state->player = X;
	state->moves_played = 1;

	load_level(&state);
}


int main() {

	GameState state;
	init_application(&state);

	while(!exit_loop) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		
		read_input(&state);
		update(&state);
		render(&state);

		if (terminal_resized()) {
			configure_parameters();
			CLEAR_SCREEN;
			background_changed = 1;
			values_changed = 1;
			if(player_out_of_bounds(&state)){
				reset_player(&state);
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &end);

		long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
		long remaining_ns = FRAME_NS - elapsed_ns;

		if (remaining_ns > 0) {
			sleep.tv_nsec = remaining_ns;
			nanosleep(&sleep, NULL);
    	}
	}

	cleanup(&state);
}
