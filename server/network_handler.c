#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network_handler.h"
#include "server_logic.h"
#include "server_data.h"
#include "socket.h"
#include "message.h"

void manage_spectators(AppContext *ctx, fd_set *fds) {
	char buffer[PACKET_SIZE];
	// accepting new spectators
	if (FD_ISSET(ctx->socket, fds)) {
		int slot = 0;
		int new_fd = accept_connection(ctx->socket);
		for (int i = 0; i < SPECTATOR_COUNT; i++) {
			if (ctx->spectator_fds[i] == -1) {
				ctx->spectator_fds[i] = new_fd;
				buffer[0] = _;
				buffer[1] = '\0';
				send_message(new_fd, buffer);
				slot = 1;
				break;
			}
		}
		if (slot){
			printf("Spectator accepted from %d\n", new_fd);
			update_spectator(ctx, new_fd);
		} else {
			send_message(new_fd, SERVER_FULL);
			close(new_fd);
			printf("Spectator rejected from %d\n", new_fd);
		}
	}
	char buf;
	// updaing spectator fd list if someone DC'd
	for(int i = 0; i < SPECTATOR_COUNT; i++) {
		if (ctx->spectator_fds[i] != -1 && FD_ISSET(ctx->spectator_fds[i], fds)) {
			if (recv(ctx->spectator_fds[i], &buf, 1, 0) <= 0) {
				printf("Spectator %d left.\n", ctx->spectator_fds[i]);
				close(ctx->spectator_fds[i]);
				ctx->spectator_fds[i] = -1;
			}
		}
	}
}

void nw_manage_connections (AppContext *ctx) {
	
	fd_set fds;
	struct timeval tv = {0,0}; // non blocking time value for select
	FD_ZERO(&fds);
	FD_SET(ctx->socket, &fds);
	int max_fd = ctx->socket;

	for(int i = 0; i < 2; i ++) {
		if(ctx->player_fds[i] == -1) {
			continue;
		}
		if (max_fd < ctx->player_fds[i]){
			max_fd = ctx->player_fds[i];
		}
		FD_SET(ctx->player_fds[i], &fds);
	}

	for(int i = 0; i < SPECTATOR_COUNT; i++) {
		if(ctx->spectator_fds[i] == -1) {
			continue;
		}
		if (max_fd < ctx->spectator_fds[i]){
			max_fd = ctx->spectator_fds[i];
		}
		FD_SET(ctx->spectator_fds[i], &fds);
	}

	if (select(max_fd+1, &fds, NULL, NULL, &tv) <= 0) {
		// if any of the file descriptors aren't set, raise the server waiting flag
		if (ctx->player_fds[0] == -1 || ctx->player_fds[1] == -1) {
			ctx->status = SERVER_WAITING;
			ctx->inbound_packet = 0;
			ctx->outbound_packet = 0;
			return;
		}
		return;
	}

	manage_spectators(ctx, &fds);

	for(int i = 0; i < 2; i++) {
		if(FD_ISSET(ctx->player_fds[i], &fds)) {
			char buf;
			if (recv(ctx->player_fds[i], &buf, 1, MSG_PEEK)<= 0) {
				printf("Player %d disconnected\n", ctx->player_fds[i]);
				close(ctx->player_fds[i]);
				ctx->player_fds[i] = -1;
			}
		}
	}

	if (ctx->player_fds[0] == -1 || ctx->player_fds[1] == -1) {
		ctx->status = SERVER_WAITING;
		ctx->inbound_packet = 0;
		ctx->outbound_packet = 0;
		return;
	}
	
	
}

void clear_buffer(AppContext *ctx) {
	char drain_buffer[PACKET_SIZE];
	for (int i = 0; i < 2; i++) {
		if (ctx->player_fds[i] != -1) {
			// clear out buffer
			while (recv(ctx->player_fds[i], drain_buffer, sizeof(drain_buffer), MSG_DONTWAIT) > 0) {
			}
		}
	}
}

void nw_wait_for_players(AppContext *ctx){
	printf("Waiting for players...\n");
	ctx->inbound_packet = 0;
	ctx->outbound_packet = 0;
	fd_set fds;
	char buffer[PACKET_SIZE];
	int max_fd = ctx->socket;

	while (ctx->player_fds[0] == -1 || ctx->player_fds[1] == -1) {
		FD_ZERO(&fds);
		FD_SET(ctx->socket, &fds);

		for(int i = 0; i < 2; i++) {
			if(ctx->player_fds[i] != -1) {
				FD_SET(ctx->player_fds[i], &fds);
				if (ctx->player_fds[i] > max_fd) {
					max_fd = ctx->player_fds[i];
				}
			}
		}

		if (select(max_fd + 1, &fds, NULL, NULL, NULL) < 0) {
			perror("select");
			continue; // js take it from the top until it works
		}
		
		// Three cases need to be handled: 
		// New player connects
		// P1 DC
		// P2 DC

		// New player
		if(FD_ISSET(ctx->socket, &fds)) {
			int new_fd = accept_connection(ctx->socket);
			
			if(ctx->player_fds[0] == -1) {
				ctx->player_fds[0] = new_fd;
				buffer[0] = X;
				buffer[1] = '\0';
				send_message(new_fd, buffer);
				printf("Player 1 connected as X on fd %d\n", new_fd);
			} else if (ctx->player_fds[1] == -1) {
				ctx->player_fds[1] = new_fd;
				buffer[0] = O;
				buffer[1] = '\0';
				send_message(new_fd, buffer);
				printf("Player 2 connected as O on fd %d\n", new_fd);
			}
		}

		// P1 DC
		if (ctx->player_fds[0] != -1 && FD_ISSET(ctx->player_fds[0], &fds)) {
			char buf;
			// peek 1 char from fd; if nothing there, they dc'd
			if (recv(ctx->player_fds[0], &buf, 1, MSG_PEEK) <= 0) {
				printf("Player 1 left while waiting for Player 2.\n");
				close(ctx->player_fds[0]);
				ctx->player_fds[0] = -1;
			}
		}

		// P2 DC
		if (ctx->player_fds[1] != -1 && FD_ISSET(ctx->player_fds[1], &fds)) {
			char buf;
			// peek 1 char from fd; if nothing there, they dc'd
			if (recv(ctx->player_fds[1], &buf, 1, MSG_PEEK) <= 0) {
				printf("Player 2 left while waiting for Player 1.\n");
				close(ctx->player_fds[1]);
				ctx->player_fds[1] = -1;
			}
		}
	}
	ctx->status = SERVER_ACTIVE;
	clear_buffer(ctx);
}

// note: this a bit weird; it doesn't take in AppContext *ctx; it's just a wrapper for send_message
// but i think other files should not send/recieve any messages directly, just use the network_handler
void message_client(int fd, char *message) {
	send_message(fd, message);
}

void message_players(AppContext *ctx, char *message) {
	send_message(ctx->player_fds[0], message);
	send_message(ctx->player_fds[1], message);
}

void message_spectators(AppContext *ctx, char *message) {
		for(int i = 0; i < SPECTATOR_COUNT; i++) {
		if (ctx->spectator_fds[i] != -1) {
			send_message(ctx->spectator_fds[i], message);
		}
	}
}

void broadcast_message(AppContext *ctx, char *message) {
	message_players(ctx, message);
	message_spectators(ctx, message);
}