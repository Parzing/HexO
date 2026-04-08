#include <stdio.h>
#include <stdlib.h>

#include "logs.h"

const char log_file[] = "logs.txt";

void log_message(char* message) {
	FILE* f = fopen(log_file, "a");
	fputs(message, f);
	fclose(f);
}

void clear_logs(){
	FILE* f = fopen(log_file, "w");
	fclose(f);
}
