#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#include <stdbool.h>

#include "socket.h"
#include "message.h"

#ifndef PORT
	#define PORT 61674
#endif

#ifndef QUIT_MSG
	#define QUIT_MSG ("quit")
#endif

void cleanup(struct sockaddr_in *self, int listenfd);


int main() {
    struct sockaddr_in *self = init_server_addr(PORT);
    int listenfd = set_up_server_socket(self, 5);
	int file_descriptor = accept_connection(listenfd);
	if (file_descriptor < 0) {
		cleanup(self, listenfd);
		exit(1);
	}

	char message[PACKET_SIZE];

	while(strcmp(message, QUIT_MSG) != 0) {
		message[0] = '\0';
		get_message(file_descriptor, message);
		if (strlen(message) != 0){
			printf("message: %s\n", message);
		}
	}

	printf("quitting\n");
	close(file_descriptor);

	cleanup(self, listenfd);
    
    return 0;
}




void cleanup(struct sockaddr_in *self, int listenfd) {
	free(self);
    close(listenfd);
}