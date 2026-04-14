#include <string.h>
#include <unistd.h>

#include "client_logic.h"
#include "socket.h"
#include "game_const.h"
#include "game_engine.h"
#include "terminal.h"
#include "hex_display.h"
#include "hex_codex.h"
#include "message.h"
#include "logs.h"

void init_game_state(ClientState *state) {
	state->key = KEY_DEFAULT;

	// force rendering
	state->background_changed = 1;
	state->values_changed = 1;

	state->pending_move = 0;
	state->server_hex = NULL;

	int lattice_center_x = (terminal_x/2-1)/2;
	int lattice_center_y = (terminal_y / 2 + 2 * lattice_center_x - 2) / 4;

	state->curr_pos = 	malloc(sizeof(Position));
	state->old_pos 	= 	malloc(sizeof(Position));
	state->anchor	=	malloc(sizeof(Position));

	state->anchor->x = 0;
	state->anchor->y = 0;

	*state->old_pos = (Position) {
		.x = lattice_center_x,
		.y = lattice_center_y
	};
	
	*state->curr_pos = (Position) {
		.x = lattice_center_x,
		.y = lattice_center_y
	};	

	state->curr_pos->x = lattice_center_x;

	state->xHexList = NULL;
	state->oHexList = NULL;
}

int init_application(AppContext *ctx, int argc, char **argv){
	char *host = (argc > 1) ? argv[1] : "127.0.0.1";
	int port = (argc > 2) ? atoi(argv[2]) : PORT;
	ctx->socket = connect_to_server(port, host);
	if(ctx->socket < 0) {
		return -1;
	} 

	configure_terminal();
	configure_parameters();
	clear_logs(CLIENT_LOG);
	signal(SIGINT, signal_handler);
	signal(SIGSEGV, signal_handler);

	char packet[PACKET_SIZE];

	int status = get_message(ctx->socket, packet);
	if(status != 0){
		log_message("Malformed initialization packet\n", CLIENT_LOG);
		return -1;
	}
	if (strcmp(packet, SERVER_FULL) == 0) {
		log_message("Server full\n", CLIENT_LOG);
		printf("Connection closed: Server full\n");
		return -1;
	}

	log_message("Packet: ", CLIENT_LOG);
	log_message(packet, CLIENT_LOG);
	log_message("\n", CLIENT_LOG);
	ctx->game.player = packet[0];

	init_game_state(&(ctx->game));
	ctx->status = STATUS_PLAYING;
	return 0;
}

void handle_system_events(AppContext *ctx){
	if (terminal_resized()) {
		configure_parameters();
		CLEAR_SCREEN;
		ctx->game.background_changed = 1;
		ctx->game.values_changed = 1;
		if(!renderable(&(ctx->game), ctx->game.curr_pos)){
			reset_cursor(&(ctx->game));
		}
	}
}

int read_key(char* buf, int i) {
	switch (buf[i]) {
		case UPL: 		return KEY_UP_L;
		case UPR: 		return KEY_UP_R;
		case DOWNL: 	return KEY_DOWN_L;
		case DOWNR: 	return KEY_DOWN_R;
		case LEFT: 		return KEY_LEFT;
		case RIGHT: 	return KEY_RIGHT;
		case SPACE: 	return KEY_PLACE;
		case ENTER: 	return KEY_PLACE;
		default: 		return KEY_DEFAULT;
	}
	return 0;
}

void process_user_input(AppContext *ctx){
	char buf[64];
	int n = read(STDIN_FILENO, buf, sizeof(buf));
	int key;
	for(int i = 0; i < n; i++){
		key = read_key(buf, i);
		if (key == KEY_DEFAULT) {
			continue;
		}
		ctx->game.key = key;
		char buffer[30];
		snprintf(buffer, sizeof(buffer), "key: %c\n", buf[i]);
		log_message(buffer, CLIENT_LOG);
		break;
	}
	
}

// simply looks at the socket and determines if there's any data to be had. 
// does not block b/c timeout for select is 0
int server_has_packet(AppContext *ctx){
	fd_set read_fds;
	struct timeval time_val = {0, 0}; // non blocking
	FD_ZERO(&read_fds);
	FD_SET(ctx->socket, &read_fds);

	int status = select(ctx->socket + 1, &read_fds, NULL, NULL, &time_val);
	if (status == -1) {
		log_message("select error", CLIENT_LOG);
		return 0;
	}
	return status;
}

// just gets the message packet from the socket.
int fetch_server_packet(AppContext *ctx, char* packet){
	if(get_message(ctx->socket, packet) != 0) {
		log_message("Disconnected with packet: ", CLIENT_LOG);
		log_message(packet, CLIENT_LOG);
		log_message("\n", CLIENT_LOG);
		ctx->status = STATUS_DISCONNECTED;
		return 0;
	} 
	log_message("Inbound: ", CLIENT_LOG);
	log_message(packet, CLIENT_LOG);
	log_message("\n", CLIENT_LOG);
	return 1;
}

// Tries to decode the packet as a hexagon. If not, it's a different type of packet 
// and we parse it as such. If it's not a type of packet we're able to accept, we write 
// an error message to our error output file.
void process_server_packet(AppContext *ctx, char* packet){
	Hexagon *hex = decode(packet);
	if (hex != NULL) {
		ctx->game.server_hex = hex;
		return;
	}

	if (strcmp(packet, X_WINS) == 0) {
		ctx->status = STATUS_X_WINS;
	} else if (strcmp(packet, O_WINS) == 0) {
		ctx->status = STATUS_O_WINS;
	} else {
		log_message("Invalid packet\n", CLIENT_LOG);
	}

}

// Updates the state of the game depending on the new move, if one exists
// If the user has placed something, updates the AppContext to signal that
// a request should be sent to the server
void update_state(AppContext *ctx) {

	if(ctx->game.server_hex != NULL) {
		update_hex_lists(&(ctx->game), ctx->game.server_hex);
		ctx->game.server_hex = NULL;
		ctx->game.values_changed = 1;
	}

	if(ctx->game.key == KEY_DEFAULT) {
		return;
	}
	if (ctx->game.key == KEY_PLACE) {
		ctx->game.pending_move = 1;
		ctx->game.key = KEY_DEFAULT;
	}
	update_cursor(&(ctx->game)); 
	ctx->game.key = KEY_DEFAULT;
}

// Attempts to send a message MAX_MESSAGE_ATTEMPTS times. If the message can't be sent,
// sets the GameStatus flag to disconnected.
void attempt_send_message(AppContext *ctx, char* message) {
	int status;
	for(int i = 0; i < MAX_MESSAGE_ATTEMPTS; i++) {
		status = send_message(ctx->socket, message);
		if (status == 0) {
			return;
		}
	}
	log_message("Unexpected error: message failed to send\n", CLIENT_LOG);
	ctx->status = STATUS_DISCONNECTED;
}

void send_move_request(AppContext *ctx) {
	Hexagon outbound_hex;
	outbound_hex.value = ctx->game.player;
	outbound_hex.pos = ctx->game.curr_pos;

	char *message = encode(&outbound_hex);
	if (message != NULL) {
		log_message("Outbound: ", CLIENT_LOG);
		log_message(message, CLIENT_LOG);
		log_message("\n", CLIENT_LOG);
		attempt_send_message(ctx, message);
		free(message);
	}

	ctx->game.pending_move = 0;
}

void render_frame(AppContext *ctx){
	render(&(ctx->game));
}

void display_conclusion(AppContext *ctx) {
	if(ctx->status == STATUS_DISCONNECTED){
		show_disconnect(&(ctx->game));
	} else {
		if(ctx->status == STATUS_X_WINS) {
			show_winner(&(ctx->game), X);
		} else {
			show_winner(&(ctx->game), O);
		}
		
	}
}

void shutdown_application(AppContext *ctx) {
	cleanup(&(ctx->game));
	reset_terminal();
	close(ctx->socket);
}
