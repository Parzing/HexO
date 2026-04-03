#include <stdio.h>
#include <stdlib.h>

const char error_file[] = "errors.txt";

void write_error(char* message) {
	FILE* f = fopen(error_file, "a");
	fputs(message, f);
	fprintf(f, "\n");
	fclose(f);
}
