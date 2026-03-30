#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <stdbool.h>

#include "socket.h"
#include "message.h"

#ifndef PORT
	#define PORT 61674
#endif

#ifndef QUIT_MSG
	#define QUIT_MSG ("quit")
#endif

#define MAX_CLIENTS 20

void cleanup(struct sockaddr_in *self, int listenfd);


int main() {
    struct sockaddr_in *self = init_server_addr(PORT);
    int listenfd = set_up_server_socket(self, 5);


	int client_fds[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++){
		client_fds[i] = -1;
	} 

	char message[PACKET_SIZE];

	while(1) {	
		fd_set pipe_set;
        FD_ZERO(&pipe_set);
        FD_SET(listenfd, &pipe_set);
		int max_fd = listenfd;

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