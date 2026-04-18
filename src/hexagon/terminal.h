#ifndef TERMINAL_H
#define TERMINAL_H

#include <signal.h>
#include <stdio.h>

#define DEFAULT_TERMINAL_X 20
#define DEFAULT_TERMINAL_Y 16

#define HIDE_CURSOR() (printf("\e[?25l"))
#define SHOW_CURSOR() (printf("\e[?25h"))
#define CURSOR_TO(x, y) (printf("\e[%d;%dH", (x) + 1, (y) + 1))
#define RESET_COLOR() (printf("\e[m"))
#define DISABLE_WRAPPING() (printf("\e[?7l"))
#define ENABLE_WRAPPING() (printf("\e[?7h"))

#define SET_LATTICE_COLOR() (printf("\033[38;5;244m"))
#define SET_CURR_COLOR() (printf("\033[38;5;254m"))

#define BOLD() (printf("\e[1m"))
#define X_COLOR() (printf("\x1B[0;31m"))
#define O_COLOR() (printf("\x1B[0;34m"))

extern volatile sig_atomic_t interrupt_received;

int terminal_resized();
void reset_terminal();
void configure_terminal();
void signal_handler(int signum);
void configure_parameters();

#endif
