#include <stdio.h>
#include <stdlib.h>

#include "hex_codex.h"
#include "game_const.h"

#define FORMAT ("%c,%d,%d")

char *encode(Hexagon *hex){
	if (hex == NULL || hex->pos == NULL) {
		return NULL;
	}

	int len = snprintf(NULL, 0, FORMAT,
                       hex->value,
                       hex->pos->x,
                       hex->pos->y);
	if (len < 0) return NULL; // not super familiar w/ snprintf
	char *message = malloc((size_t)len + 1);
    if (message == NULL) {
        return NULL;
    }

	snprintf(message, (size_t)len + 1, FORMAT,
             hex->value,
             hex->pos->x,
             hex->pos->y);
	return message;
}


Hexagon *decode(char* message){
	if (message == NULL) {
        return NULL;
    }

    char value;
    int x, y;

	if (sscanf(message, FORMAT, &value, &x, &y) != 3) {
        return NULL;
    }

    Hexagon *hex = malloc(sizeof(Hexagon));
    if (hex == NULL) {
        return NULL;
    }

	hex->pos = malloc(sizeof(Position));
    if (hex->pos == NULL) {
        free(hex);
        return NULL;
    }

    hex->value = value;
    hex->pos->x = x;
    hex->pos->y = y;

    return hex;
}