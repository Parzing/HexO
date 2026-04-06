#ifndef HEX_CODEX_H
#define HEX_CODEX_H

#include "game_const.h"

// Returns an encoding of the hexagon. The caller should remember to free() the message.
char *encode(Hexagon *hex);
// Returns a decoding of the message. The caller should remember to free() the hexagon.
Hexagon *decode(char* message);

#endif