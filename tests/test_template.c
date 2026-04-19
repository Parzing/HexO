#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "tester.h"

bool foo() {
	return true;
}

bool bar() {
	return false;
}
bool baz() {
	return true;
}

int main(int argc, char** argv) {
	TestCase tests[] = {
		{foo, "foo", "Tests if 5 < 6"},
		{bar, "bar", "Tests if 6 < 5"},
		{baz, "baz", "Tests of 6 < 9"}
	};

	int test_num = sizeof(tests)/sizeof(TestCase);
	run_tests(tests, test_num, argc, argv);
}