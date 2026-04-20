#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "terminal.h"
#include "render.h"

void configure_parameters(RenderState *render_state) {
	render_state->terminal_x = 100;
	render_state->terminal_y = 100;
}

int terminal_resized() {
	return 1;
}

void configure_terminal() {
	return;
}

void reset_terminal() {
	return;
}

void signal_handler(int signum) {
	signum++; // lmao
}
