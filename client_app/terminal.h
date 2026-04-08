#ifndef TERMINAL_H
#define TERMINAL_H

#define DEFAULT_TERMINAL_X 20
#define DEFAULT_TERMINAL_Y 16

#include <signal.h>

extern int terminal_x;
extern int terminal_y;
extern volatile sig_atomic_t interrupt_received;

int terminal_resized();
void reset_terminal();
void configure_terminal();
void signal_handler(int signum);
void configure_parameters();

#endif