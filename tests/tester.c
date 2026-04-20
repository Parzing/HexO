#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "tester.h"

#define RESET_BOLD() 	(printf("\e[22m"))
#define BOLD() 			(printf("\e[1m"))

#define RESET_COLOR() 	(printf("\e[m"))
#define PASSED_COLOR() 	(printf("\e[38;5;47m"))
#define FAILED_COLOR() 	(printf("\e[38;5;196m"))

int num_passed;
int num_tests;

typedef struct {
	bool failing;
	bool help;
	bool passing;
	bool quiet;
	bool verbose;
	bool invalid;
} TestConfig;


/*
 * Sets configuration of testing based on the arguments
 */
void set_config(TestConfig *config, int argc, char** argv) {
	if (argc == 1) {
		config->passing = true;
		config->failing = true;
		return;
	}

	int opt;
	// afhpqv: just a list of possible options, -a, -f, -p, etc
	while ((opt = getopt(argc, argv, "afhpqv")) != -1) {
		switch (opt)
		{
			case 'a': config->passing 	= true;
					  config->failing 	= true; break;
			case 'f': config->failing 	= true; break;
			case 'h': config->help 		= true; break;
			case 'p': config->passing 	= true; break;
			case 'q': config->quiet 	= true; break;
			case 'v': config->verbose 	= true; break;
			default: 
				config->invalid = true;
				return;
		}
	}
	if (!config->passing && !config->failing) {
        config->passing = true;
        config->failing = true;
    }

	if (config->quiet && config->verbose) {
		config->invalid = true;
	}
}

void display_output(TestConfig config, bool passed, TestCase curr_test){
	if(config.quiet) return;

	if (passed && config.passing) {
		printf("Testing %s... ", curr_test.name);
		PASSED_COLOR();
		printf("Passed!\n");
		RESET_COLOR();
		if (config.verbose) {
			printf("\t%s\n", curr_test.description);
		}
	} else if (!passed && config.failing){
		printf("Testing %s... ", curr_test.name);
		FAILED_COLOR();
		printf("Failed.\n");
		RESET_COLOR();
		if (config.verbose) {
			printf("\t%s\n", curr_test.description);
		}
	}
}


void run_tests(TestCase *tests, int test_count, int argc, char** argv) {
	TestConfig config = {0};

	set_config(&config, argc, argv);
	if (config.invalid) {
		printf("Usage: %s <flags>\n", argv[0]);
		return;
	}
	if(config.help) {
		printf("Usage: %s <flags>\n", argv[0]);
		printf("	-a: Show all tests\n"
				"	-f: Show failing tests\n"
				"	-h: Show this menu\n"
				"	-p: Show passing tests\n"
				"	-q: Quiet mode. No tests shown\n"
				"	-v: Show verbose (descriptions)\n");
		return;
	}

	char *name = strrchr(argv[0], '/');  // find last '/'
    name = name ? name + 1 : argv[0];

	bool passed;
	num_passed = 0;
	num_tests = test_count;

	if (!config.quiet) {
		printf("==============================================\n");
		printf("Beginning tests for ");
		BOLD();
		printf("%s\n\n", name);
		RESET_BOLD();
	}

	for (int i = 0; i < num_tests; i++) {
		passed = tests[i].function();
		if (passed) {
			num_passed++;
		}
		display_output(config, passed, tests[i]);
	}

	if(config.quiet) {
		if(num_passed == num_tests) PASSED_COLOR();
		else FAILED_COLOR();

		printf("%s: %d/%d\n", name, num_passed, num_tests);
		RESET_COLOR();
	} else {
		printf("\nTests ended for ");
		BOLD();
		printf("%s\n", name);
		RESET_BOLD();
		printf("Tests passed: %d/%d\n", num_passed, num_tests);
		printf("==============================================\n");
	}
}