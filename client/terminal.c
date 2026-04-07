#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "game_const.h"
#include "terminal.h"


static struct termios old_termios, new_termios;
static struct winsize old_winsize, new_winsize;

int terminal_x = DEFAULT_TERMINAL_X;
int terminal_y = DEFAULT_TERMINAL_Y;
volatile sig_atomic_t interrupt_received = 0;

void configure_parameters() {

	//upon failure, default to old window size
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &new_winsize) == -1){

		return;
	}

	terminal_x = new_winsize.ws_row;
	terminal_y = new_winsize.ws_col;
	
	terminal_x = (terminal_x < 1) ? 1 : terminal_x;
	terminal_y = (terminal_y < 1) ? 1 : terminal_y;
	old_winsize = new_winsize;
}

int terminal_resized() {
	
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &new_winsize) == -1){
		return 0;
	}

	if (old_winsize.ws_col != new_winsize.ws_col ||
		old_winsize.ws_row != new_winsize.ws_row) {
		return 1;
	}
	return 0;
}

void configure_terminal() {
	DISABLE_WRAPPING;
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
	ENABLE_WRAPPING;
	RESET_COLOR;
	SHOW_CURSOR;
	CLEAR_SCREEN;
	CURSOR_TO(0, 0);
	fflush(stdout);
	tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

void signal_handler(__attribute__((unused)) int signum) {
	if (signum == SIGSEGV) {
		reset_terminal();
		fprintf(stderr, "Fatal: Segmentation Fault\n");
		exit(1);
	}
	interrupt_received = 1;
}
