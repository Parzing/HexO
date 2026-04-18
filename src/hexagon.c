#include <stdlib.h>
#include "hexagon.h"
#include "common.h"

void push_to(HexagonList **head, Hexagon *hex) {
    HexagonList *new_node = malloc(sizeof(HexagonList));
    new_node->hex = hex;
    new_node->next = *head;
    *head = new_node;
}

Hexagon *get_hexagon(HexagonList *hex_list, Position pos) {
	HexagonList *curr = hex_list;
	while (curr != NULL) {
		if (curr->hex->pos.x == pos.x && curr->hex->pos.y == pos.y) {
			return curr->hex;
		}
		curr = curr->next;
	}
	return NULL;
}

Hexagon *ensure_hexagon(HexagonList *list, Position pos) {
	Hexagon *hex = get_hexagon(list, pos);
	if (hex != NULL) {
		return hex;
	}

	hex = malloc(sizeof(Hexagon));
	hex->value = _;
	hex->pos.x = pos.x;
	hex->pos.y = pos.y;

	return hex;
}
