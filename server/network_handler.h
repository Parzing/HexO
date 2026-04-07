#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include "game_const.h"
#include "server_data.h"

void nw_manage_connections (AppContext *ctx);
void nw_wait_for_players(AppContext *ctx);
void broadcast_message(AppContext *ctx, char *message);

#endif