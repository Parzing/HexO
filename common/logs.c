#include <stdio.h>
#include <stdlib.h>

#include "logs.h"

void log_message(char* message, char* path) {
	FILE* f = fopen(path, "a");
	fputs(message, f);
	fclose(f);
}

void clear_logs(char *path){
	FILE* f = fopen(path, "w");
	fclose(f);
}
