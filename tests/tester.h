#ifndef TESTER_H
#define TESTER_H

#include <stdbool.h>

typedef bool (*func_ptr)(void);

typedef struct {
	func_ptr function;
	char *name;
	char *description;
} TestCase;


// Runs tests in the tests array. Every function must return true or false if the test passes or fails.
void run_tests(TestCase *tests, int test_count, int argc, char** argv);

#endif
