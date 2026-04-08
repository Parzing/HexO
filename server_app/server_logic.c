#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include "socket.h"
#include "logs.h"
#include "message.h"
#include "hex_codex.h"

#include "server_logic.h"
#include "network_handler.h"
#include "server_data.h"
#include "game_engine.h"

// im ngl i kinda dont know what all this is i just looked it up
void display_hostname(int port) {
	struct ifaddrs *ifaddr, *curr;
	char *ip;

	if(getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return;
	}

	curr = ifaddr;

	while(curr != NULL) {
		if (curr->ifa_addr == NULL || curr->ifa_addr->sa_family != AF_INET) {
			curr = curr->ifa_next;
			continue;
		}
		// note: curr->ifa_addr is sockaddr, but casting is OK bc polymorphism
		struct sockaddr_in *ip_addr = (struct sockaddr_in*) curr->ifa_addr; 
		ip = inet_ntoa(ip_addr->sin_addr);

		if(strcmp(ip, "127.0.0.1") != 0) {
			printf("Server started with IP %s on port %d\n", ip, port);
			break;
		}
		curr = curr->ifa_next;
	}
	
	freeifaddrs(ifaddr);
}

int init_server_application(AppContext *ctx, int port){
	struct sockaddr_in *self = init_server_addr(port);
	ctx->socket = set_up_server_socket(self, 5);
	clear_logs(SERVER_LOG);
	ctx->player_fds[0] = -1;
	ctx->player_fds[1] = -1;
	for (int i = 0; i < SPECTATOR_COUNT; i++) {
		ctx->spectator_fds[i] = -1;
	}

	ctx->status = SERVER_WAITING;
	ctx->inbound_packet = 0;
	ctx->outbound_packet = 0;

	ctx->game.player = X;
	ctx->game.winner = _;
	ctx->game.num_moves = 1;	// x goes only once
	ctx->game.oHexList = NULL;
	ctx->game.xHexList = NULL;
	display_hostname(port);
	log_message("Server initialized.\n", SERVER_LOG);

	return 0;
}

// Handles new connections/disconnections
void manage_connections(AppContext *ctx) {
	nw_manage_connections (ctx);
}

void wait_for_players(AppContext *ctx){
	nw_wait_for_players(ctx);
}

// Note: Currently, updates all new players with entire board state.
void update_new_players(AppContext *ctx){
	HexagonList *xCurr = ctx->game.xHexList;
	HexagonList *oCurr = ctx->game.oHexList;
	char *buffer;

	while(xCurr != NULL) {
		buffer = encode(xCurr->hex);
		broadcast_message(ctx, buffer);
		free(buffer);
		xCurr = xCurr->next;
	}
	while(oCurr != NULL) {
		buffer = encode(oCurr->hex);
		broadcast_message(ctx, buffer);
		free(buffer);
		oCurr = oCurr->next;
	}
	ctx->inbound_packet = 0;
	ctx->outbound_packet = 0;
	memset(ctx->move_packet, 0, PACKET_SIZE);

	ctx->status = SERVER_ACTIVE;

}

void update_spectator(AppContext *ctx, int fd) {
	HexagonList *xCurr = ctx->game.xHexList;
	HexagonList *oCurr = ctx->game.oHexList;
	char *buffer;

	while(xCurr != NULL) {
		buffer = encode(xCurr->hex);
		message_client(fd, buffer);
		free(buffer);
		xCurr = xCurr->next;
	}
	while(oCurr != NULL) {
		buffer = encode(oCurr->hex);
		message_client(fd, buffer);
		free(buffer);
		oCurr = oCurr->next;
	}
}

void process_client_packets(AppContext *ctx){
	fd_set read_fds;
	struct timeval time_val = {0, 0}; // non blocking
	int max_fd = ctx->player_fds[0] > ctx->player_fds[1] ? ctx->player_fds[0] : ctx->player_fds[1];
	if (max_fd == -1) return;

	FD_ZERO(&read_fds);
	if (ctx->player_fds[0] != -1){
		FD_SET(ctx->player_fds[0], &read_fds);
	}
	if (ctx->player_fds[0] != -1){
		FD_SET(ctx->player_fds[1], &read_fds);
	}

	int status = select(max_fd + 1, &read_fds, NULL, NULL, &time_val);
	
	if (status == -1) {
		log_message("select error", SERVER_LOG);
		return;
	}

	for(int i = 0; i < 2; i++){
		if (FD_ISSET(ctx->player_fds[i], &read_fds)) {
			status = get_message(ctx->player_fds[i], ctx->move_packet);
			if (status < 0) return;
			char buffer[256];
			snprintf(buffer, sizeof(buffer), "Packet: %s\n", ctx->move_packet);
			log_message(buffer, SERVER_LOG);
			ctx->inbound_packet = 1;
			return;
		}
	}
}

void update_server_state(AppContext *ctx){
	if(!ctx->inbound_packet) {
		return;
	}

	Hexagon *hex = decode(ctx->move_packet);
	if (hex == NULL) return;

	ctx->inbound_packet = 0;
	ctx->outbound_packet = valid_move(&(ctx->game), hex);
	if(!ctx->outbound_packet) {
		free(hex->pos);
		free(hex);
		return;
	}
	char buffer[256];
	snprintf(buffer, sizeof(buffer), "Move: %s\n", ctx->move_packet);
	log_message(buffer, SERVER_LOG);
	printf("%s", buffer);

	process_move(&(ctx->game), hex);
}

void broadcast_updates(AppContext *ctx){
	if (!ctx->outbound_packet) {
		return;
	}
	ctx->outbound_packet = 0;

	char* buffer = encode(ctx->game.curr_move);

	broadcast_message(ctx, buffer);

	if (has_winner(&(ctx->game))) {
		if(ctx->game.winner == X){
			buffer = X_WINS;
		} else {
			buffer = O_WINS;
		}
		printf("%s\n", buffer);
		log_message(buffer, SERVER_LOG);
		broadcast_message(ctx, buffer);
		ctx->status = SERVER_SHUTDOWN;
		return;
	}
	free(buffer);
}

void cleanup_server(AppContext *ctx) {
	close(ctx->socket);

	for(int i = 0; i < 2; i++){
		if(ctx->player_fds[i] != -1){
			close(ctx->player_fds[i]);
		}
	}

	for(int i = 0; i < SPECTATOR_COUNT; i++) {
		if(ctx->spectator_fds[i] != -1){
			close(ctx->spectator_fds[i]);
		}
	}
	cleanup_game(&(ctx->game));
}