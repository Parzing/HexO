#include <stdio.h>
#include <stdlib.h>

#include "client_logic.h"

#define FORMAT ("%u,%d,%d")

char *encode(Hexagon *hex){
	if (hex == NULL || hex->pos == NULL) {
		return NULL;
	}

	int len = snprintf(NULL, 0, FORMAT,
                       (unsigned char) hex->value,
                       hex->pos->x,
                       hex->pos->y);
	if (len < 0) return NULL; // not super familiar w/ snprintf
	char *message = (char *)malloc((size_t)len + 1);
    if (message == NULL) {
        return NULL;
    }

	snprintf(message, (size_t)len + 1, FORMAT,
             (unsigned char)hex->value,
             hex->pos->x,
             hex->pos->y);
	return message;
}


Hexagon *decode(char* message){
	if (message == NULL) {
        return NULL;
    }

    unsigned int value;
    int x, y;

	if (sscanf(message, FORMAT, &value, &x, &y) != 3) {
        return NULL;
    }

    Hexagon *hex = (Hexagon *)malloc(sizeof(Hexagon));
    if (hex == NULL) {
        return NULL;
    }

	hex->pos = (Position *)malloc(sizeof(Position));
    if (hex->pos == NULL) {
        free(hex);
        return NULL;
    }

    hex->value = (char)value;
    hex->pos->x = x;
    hex->pos->y = y;

    return hex;
}