#ifndef CLIENT_LOGIC_H
#define CLIENT_LOGIC_H

#define DEFAULT_PORT 61674

typedef struct Position{
	int x;
	int y;
} Position;

typedef struct Hexagon{
	char value;
	struct Position* pos;
} Hexagon;

typedef struct HexagonList {
	struct Hexagon *hex;
	struct HexagonList *next;
} HexagonList;

// pending_move is a little awkward. I feel as though the AppContext should 
// have some sort of inbound_packet flag, but then how do you access it from 
// inside the client_state? ClientState shouldn't handle any sort of network
// packetry, but i'm not sure how else to do it. Compromise, eh?
typedef struct {
	int key;
	char player; // note: this will never change

	int background_changed;	// global vars moved to inside the client state; 
	int values_changed;		// it's not like the server is rendering for the client lol

	int pending_move;	// 1 if there is a move we need to send to the server
	struct Hexagon *server_hex;

	struct Position *curr_pos;
	struct Position *old_pos;
	struct Position *anchor;

	struct HexagonList *xHexList;
	struct HexagonList *oHexList;
} ClientState;

typedef enum {
    STATUS_PLAYING,
    STATUS_WINNER,
    STATUS_LOSER,
    STATUS_DISCONNECTED
} GameStatus;

typedef struct {
    int socket;
    ClientState game;    // The board, lists, and cursor
    GameStatus status;
} AppContext;

int  init_application(AppContext *ctx, int argc, char **argv);
void handle_system_events(AppContext *ctx);
void process_user_input(AppContext *ctx);
int  server_has_packet(AppContext *ctx);
int  fetch_server_packet(AppContext *ctx, char* packet);
void process_server_packet(AppContext *ctx, char* packet);
void update_state(AppContext *ctx);
void send_move_request(AppContext *ctx);
void render_frame(AppContext *ctx);
void shutdown_application(AppContext *ctx);

#endif