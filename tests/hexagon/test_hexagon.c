#include <stdio.h>
#include <stdlib.h>

#include "../../src/hexagon/hexagon.h"
#include "../../src/hexagon/common.h"
#include "../tester.h"

bool test_push_to_null(void) {
	HexagonList *list = NULL;
	Hexagon *hex = malloc(sizeof(Hexagon));
	hex->value = X;
	hex->pos = (Position) {
		.x = 1,
		.y = 1
	};

	push_to(&list, hex);
	bool success = true;
	if (hex == NULL) {
		success = false;
	} else {
		free(hex);
	}
	if(list == NULL) {
		success = false;
	} else {
		free(list);
	}
	return success;
}

bool test_push_to_list(void) {
	HexagonList *list = NULL;
	Hexagon *hex1 = malloc(sizeof(Hexagon));
	hex1->value = X;
	hex1->pos = (Position) {
		.x = 1,
		.y = 1
	};

	Hexagon *hex2 = malloc(sizeof(Hexagon));
	hex2->value = O;
	hex2->pos = (Position) {
		.x = 0,
		.y = 0
	};

	push_to(&list, hex1);
	push_to(&list, hex2);

	bool success = true;
	if (hex1 == NULL) success = false;
 	else free(hex1);
	if (hex2 == NULL) success = false;
 	else free(hex2);

	if(list->next == NULL) {
		success = false;
	} else {
		free(list->next);
	}

	if(list == NULL) {
		success = false;
	} else {
		free(list);
	}

	return success;
}


bool test_get_hexagon_from_null(void){
	HexagonList *list = NULL;
	Hexagon *hex = get_hexagon(list, (Position) {1, 1});
	if (hex != NULL) {
		return false;
	}
	return true;
}

bool test_get_hexagon_from_list(void){
	HexagonList *list = NULL;
	Hexagon *hex1 = malloc(sizeof(Hexagon));
	hex1->value = X;
	hex1->pos = (Position) {
		.x = 1,
		.y = 1
	};
	Hexagon *hex2 = malloc(sizeof(Hexagon));
	hex2->value = O;
	hex2->pos = (Position) {
		.x = 0,
		.y = 0
	};
	
	push_to(&list, hex1);
	push_to(&list, hex2);

	Hexagon *hex3 = get_hexagon(list, (Position) {1, 1});

	bool success = true;
	if (hex1 != hex3) success = false;

	if (hex1 == NULL) success = false;
 	else free(hex1);
	if (hex2 == NULL) success = false;
 	else free(hex2);
	if (hex3 == NULL) success = false;

	if(list->next == NULL) success = false;
	else free(list->next);

	if(list == NULL) success = false;
	else free(list);

	return success;
}

bool test_ensure_hexagon(void) {
	Hexagon *hex = ensure_hexagon(NULL, (Position) {0,0});
	if (hex == NULL) {
		return false;
	} else {
		free(hex);
		return true;
	}

}

int main(int argc, char **argv) {
	TestCase tests[] = {
		{test_push_to_null, "test_push_to_null", "Tests if pushing a hexagon to a null list correctly works"},
		{test_push_to_list, "test_push_to_list", "Tests pushing two hexagons to a list correctly pushes them both"},
		{test_get_hexagon_from_null, "test_get_hexagon_from_null", "Tests retrieving a hexagon from an empty list"},
		{test_get_hexagon_from_list, "test_get_hexagon_from_list", "Tests retrieving a hexagon from a list that contains it"},
		{test_ensure_hexagon, "test_ensure_hexagon", "Tests ensuring a hexagon in a empty list successfully creates the hexagon."}
	};

	int test_num = sizeof(tests)/sizeof(TestCase);
	run_tests(tests, test_num, argc, argv);
}