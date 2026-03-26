#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "socket.h"
#include "message.h"
#ifndef PORT
	#define PORT 61674
#endif

#define QUITTING ("quit")
#define TIMES 5 // number of times to send the NOT_QUIT
#define MINCHARS 3
#define MAXCHARS 7


void send_to_socket(int socket, char *message);

int main(int argc, char **argv) {
    // if (argc != 2) {

    //     fprintf(stderr, "Usage: client hostname\n");
    //     exit(1);
    // }

    int soc = connect_to_server(PORT, "127.0.0.1");
	

	char message[PACKET_SIZE];
	message[0] = '\0';
	
	while(strcmp(message, QUITTING)!= 0) {
		printf("> ");
		fgets(message, 100, stdin);
		int len = strlen(message);
		if(message[len-1] == '\n') {
			message[len-1] = '\0';
		}
		send_message(soc, message);
	}
    close(soc);
    return 0;
}

void send_to_socket(int socket, char *message) {
	write(socket, &PACKET_START, 1);
	write(socket, message, strlen(message));
	write(socket, &PACKET_END, 1);
}
