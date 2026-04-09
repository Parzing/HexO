#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include "logs.h"

#define SERVER_LOG ("../server/logs.txt")
void timing(void (*function)(void)) {
    struct timespec start = {0};
    struct timespec end = {0};

    clock_gettime(CLOCK_MONOTONIC, &start);
    function();
    clock_gettime(CLOCK_MONOTONIC, &end);

    long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L +
                      (end.tv_nsec - start.tv_nsec);
    double elapsed_sec = (double)elapsed_ns / 1000000000.0;

    printf("Function took %ld ns or %lf sec\n", elapsed_ns, elapsed_sec);
	printf("in 1,000,000 assignments, average time is: %lf ns or %lf sec", elapsed_ns/1000000.0, elapsed_sec/1000000.0);
}


int main() {
	char *message = "ballsacks";
	log_message(message, SERVER_LOG);
}


