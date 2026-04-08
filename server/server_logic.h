#ifndef SERVER_LOGIC_H
#define SERVER_LOGIC_H

#include "game_const.h"
#include "server_data.h"

int init_server_application(AppContext *ctx, int port);
void manage_connections(AppContext *ctx);
void wait_for_players(AppContext *ctx);
void update_new_players(AppContext *ctx);
void update_spectator(AppContext *ctx, int fd);
void process_client_packets(AppContext *ctx);
void update_server_state(AppContext *ctx);
void broadcast_updates(AppContext *ctx);
void cleanup_server(AppContext *ctx);

#endif