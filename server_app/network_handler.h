#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include "game_const.h"
#include "server_data.h"

void nw_manage_connections (AppContext *ctx);
void nw_wait_for_players(AppContext *ctx);
void message_client(int *fd, char *message);
void message_players(AppContext *ctx, char *message);
void message_spectators(AppContext *ctx, char *message);
void broadcast_message(AppContext *ctx, char *message);

#endif