#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "tester.h"

#define RESET_COLOR() (printf("\e[m"))
#define PASSED_COLOR() (printf("\e[38;5;47m"))
#define FAILED_COLOR() (printf("\e[38;5;196m"))

int num_passed;
int num_tests;

typedef struct {
	bool passing;
	bool failing;
	bool verbose;
	bool help;
	bool invalid;
} TestConfig;


/*
 * Sets configuration of testing based on the arguments
 */
void set_config(TestConfig *config, int argc, char** argv) {
	int opt;
	// afpvh: just a list of possible options, -a, -f, -p, etc
	while ((opt = getopt(argc, argv, "afpvh")) != -1) {
		switch (opt)
		{
			case 'a':
				config->passing = true;
				config->failing = true;
				break;
			case 'p':
				config->passing = true;
				break;
			case 'f':
				config->failing = true;
				break;
			case 'v':
				config->verbose = true;
				break;
			case 'h':
				config->help = true;
				break;
			default:
				config->invalid = true;
				return;
		}
	}
}

void run_tests(TestCase *tests, int test_count, int argc, char** argv) {
	TestConfig config;
	if(argc > 1){
		set_config(&config, argc, argv);
		if(config.invalid) {
			printf("Usage: %s <flags>\n", argv[0]);
			return;
		}
	}
	if (argc == 1 || (!config.passing && !config.failing)){
		config.passing = true;
		config.failing = true;
	}

	if(config.help) {
		printf("Usage: %s <flags>\n", argv[0]);
		printf("\t-a: Show all tests\n");
		printf("\t-f: Show failing tests\n");
		printf("\t-h: Show this menu\n");
		printf("\t-p: Show passing tests\n"); 
		printf("\t-v: Show verbose (descriptions)\n");
		return;
	}
	bool last_passed;
	num_passed = 0;
	num_tests = test_count;
	printf("==============================================\n");
	printf("Beginning tests for %s\n", argv[0]);
	printf("==============================================\n");
	for (int i = 0; i < num_tests; i++) {
		last_passed = tests[i].function();
		if (last_passed) {
			num_passed++;
		}

		if (last_passed && config.passing) {
			printf("Testing %s... ", tests[i].name);
			PASSED_COLOR();
			printf("Passed!\n");
			RESET_COLOR();
			if (config.verbose) {
				printf("\t%s\n", tests[i].description);
			}
		} else if (!last_passed && config.failing){
			printf("Testing %s... ", tests[i].name);
			FAILED_COLOR();
			printf("Failed.\n");
			RESET_COLOR();
			if (config.verbose) {
				printf("\t%s\n", tests[i].description);
			}
		}

	}
	printf("==============================================\n");
	printf("Tests ended for %s\n", argv[0]);
	printf("Tests passed: %d/%d\n", num_passed, num_tests);
	printf("==============================================\n");
}