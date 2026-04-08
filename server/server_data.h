#ifndef SERVER_DATA_H
#define SERVER_DATA_H

#include "game_const.h"
#include "message.h"

#define DEFAULT_PORT 61674
#define SPECTATOR_COUNT 1

typedef struct {
	char player;	// curr player
	char winner;	// either X or O or _

	int num_moves;	// num moves of current player

	struct Hexagon *curr_move;	// current move to be processed

	struct HexagonList *xHexList;
	struct HexagonList *oHexList;
} ServerState;

typedef enum {
	SERVER_WAITING,
    SERVER_ACTIVE,
    SERVER_SHUTDOWN
} ServerStatus;

typedef struct {
    int socket;
	int player_fds[2];
	int spectator_fds[SPECTATOR_COUNT];

	int inbound_packet;	// 1 if there is an incoming move we need to process
	int outbound_packet;
	char move_packet[PACKET_SIZE];

    ServerStatus status;
    ServerState game;    // The board, lists, and cursor
} AppContext;

#endif