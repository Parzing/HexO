#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "client_logic.h"
#include "message.h"
#include "terminal.h"
#include "logs.h"

#ifndef PORT
	#define PORT 61674
#endif

#define FRAME_NS 16666667

#define QUITTING ("quit")

void limit_frame_rate(struct timespec *start){
	struct timespec end = {};
	struct timespec sleep = {};

	clock_gettime(CLOCK_MONOTONIC, &end);

	long elapsed_ns = (end.tv_sec - start->tv_sec) * 1000000000L + (end.tv_nsec - start->tv_nsec);
	long remaining_ns = FRAME_NS - elapsed_ns;

	if (remaining_ns > 0) {
		sleep.tv_sec = 0;
		sleep.tv_nsec = remaining_ns;
		nanosleep(&sleep, NULL);
	}
}

int main(int argc, char **argv) {

	AppContext ctx;
	struct timespec frame_start;
	char packet[PACKET_SIZE];
	
	if(init_application(&ctx, argc, argv) != 0) {
		return 1;
	}
	log_message("Initialization complete.\n");
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "Client is now player %c\n", ctx.game.player);
	log_message(buffer);
	log_message("\n");

	while (ctx.status == STATUS_PLAYING) {
		if(interrupt_received) {
			ctx.status = STATUS_DISCONNECTED;
			continue;
		}

		clock_gettime(CLOCK_MONOTONIC, &frame_start);

		handle_system_events(&ctx); // terminal resizing
		process_user_input(&ctx);
		
		if(server_has_packet(&ctx)) {
			if(fetch_server_packet(&ctx, packet)){
				process_server_packet(&ctx, packet);
			}
		}
		update_state(&ctx);

		if (ctx.game.pending_move) {
			send_move_request(&ctx);
		}

		render_frame(&ctx);

		limit_frame_rate(&frame_start);
	}

	display_winner(&ctx);

	shutdown_application(&ctx);

	return 0;
}

