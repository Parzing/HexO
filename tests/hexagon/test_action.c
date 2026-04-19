#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "../../src/hexagon/action.h"
#include "../tester.h"

bool test_right(void) {
	int pipe_fds[2];
	const char *input = "dfe";
	if(pipe(pipe_fds) == -1) {
		perror("no pipe game\n");
		return false;
	}
	write(pipe_fds[1], input, strlen(input));
	dup2(pipe_fds[0], STDIN_FILENO);

	close(pipe_fds[0]);
	close(pipe_fds[1]);

	Action action = get_user_action();
	return action == ACT_RIGHT;
}

bool test_left(void) {
	int pipe_fds[2];
	const char *input = "afe";
	if(pipe(pipe_fds) == -1) {
		perror("no pipe game\n");
		return false;
	}
	write(pipe_fds[1], input, strlen(input));
	dup2(pipe_fds[0], STDIN_FILENO);

	close(pipe_fds[0]);
	close(pipe_fds[1]);

	Action action = get_user_action();
	return action == ACT_LEFT;
}


int main(int argc, char** argv) {
	TestCase tests[] = {
		{test_right, "test_right", "Tests if the action transmitted by user input \"dfe\" is ACT_RIGHT"},
		{test_left, "test_left", "Tests if the action transmitted by user input \"afe\" is ACT_LEFT"}
	};

	int test_num = sizeof(tests)/sizeof(TestCase);
	run_tests(tests, test_num, argc, argv);
}