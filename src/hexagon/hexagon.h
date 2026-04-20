#ifndef HEXAGON_H
#define HEXAGON_H

// x is <->
// y is bottom left to top right
typedef struct Position {
	int x;
	int y;
} Position;

typedef struct Hexagon {
	char value;
	struct Position pos;
} Hexagon;

typedef struct HexagonList {
	struct Hexagon *hex;
	struct HexagonList *next;
} HexagonList;

/*
 * Add the given hexagon to the list
 */
void push_to(HexagonList **head, Hexagon *hex);

/*
 * Get the hexagon at the given position; if one does not exist, returns NULL
 */
Hexagon* get_hexagon(HexagonList *hex_list, Position pos);

/*
 * Return a hexagon at the given position. If one exists in the list, return
 * it; otherwise, malloc and return an empty hexagon with the given position.
 */
Hexagon *ensure_hexagon(HexagonList *list, Position pos);

#endif
