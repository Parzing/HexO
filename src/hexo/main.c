#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "../input/input.h"
#include "display.h"
#include "../game_engine"


int counter = 0;

typedef struct {
	bool is_local;
	char* ip_address;
	int port;
	// more to come
} Options;

void play_game(Options *options) {
	AppContext ctx;
	init_application(&ctx, options);
	while(ctx.playing) {
		get_user_input(&ctx);
		process_input(&ctx);
		if(options->is_local) {
			update_server(&ctx);
		} else {
			update_local(&ctx);
		}
		render(&ctx);
	}
	display_end_result(&ctx);
}

void handle_menu(Options *options){
	options->ip_address = "127.0.0.1";
	options->port = 61674;
}

bool ask_play_again() {
	if(counter == 2) {
		return false;
	}
	counter++;
	return true;
}


int main() {
	Options options;

	while (1) {
		handle_menu(&options);
		play_game(&options);
		if (!ask_play_again())
			break;
	}

	return 0;
}

