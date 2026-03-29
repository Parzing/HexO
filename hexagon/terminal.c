
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include 
#include "common.h"
#include "terminal.h"

static struct termios old_termios, new_termios;
static struct winsize old_winsize, new_winsize;

void configure_grid_dimensions()

void configure_terminal() {
	tcgetattr(STDIN_FILENO, &old_termios);
	new_termios = old_termios;

	new_termios.c_lflag &= ~(ICANON | ECHO); // no echo + no canonical
	new_termios.c_cc[VMIN] = 0; // polling read
	new_termios.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

	HIDE_CURSOR;
	CLEAR_SCREEN;

	atexit(reset_terminal);
}

void reset_terminal() {
	RESET_COLOR;
	SHOW_CURSOR;
	CLEAR_SCREEN;
	CURSOR_TO(0, 0);
	fflush(stdout);
	tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

void signal_handler(__attribute__((unused)) int signum) {
	exit_loop = 1;
}
