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
void broadcast(char* message, int* client_soc);

int main() {
    struct sockaddr_in *self = init_server_addr(PORT);
    int listenfd = set_up_server_socket(self, 5);


	int client_fds[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++){
		client_fds[i] = -1;
	} 

	char message[PACKET_SIZE];
	printf("Server started on port %d. Waiting for connections...\n", PORT);

	while(1) {	
		fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(listenfd, &read_fds);
		FD_SET(STDIN_FILENO, &read_fds);
		int max_fd = STDIN_FILENO > listenfd ? STDIN_FILENO : listenfd;

		for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] != -1) {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd) {
                    max_fd = client_fds[i];
                }
            }
        }
		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            continue;
        }

		if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(message, PACKET_SIZE, stdin) != NULL) {
                // Clean up newline
                size_t len = strlen(message);
                if (len > 0 && message[len - 1] == '\n') {
                    message[len - 1] = '\0';
                }

                // Broadcast the manual message to all clients
                if (strlen(message) > 0) {
                    printf("sent: %s\n", message);
                    broadcast(message, client_fds);
                }
                printf("> ");
                fflush(stdout);
            }
        }

		if (FD_ISSET(listenfd, &read_fds)) {
            int new_client = accept_connection(listenfd);
            if (new_client >= 0) {
                // Find a spot in our array for the new client
                int found = 0;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_fds[i] == -1) {
                        client_fds[i] = new_client;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    fprintf(stderr, "Server full, rejecting client.\n");
                    close(new_client);
                }
            }
        }

		for (int i = 0; i < MAX_CLIENTS; i++) {
            int client_soc = client_fds[i];
            if (client_soc != -1 && FD_ISSET(client_soc, &read_fds)) {
                
                // Try to read a message using the protocol
                if (get_message(client_soc, message) < 0) {
                    // Error or Disconnection
                    printf("Client on socket %d disconnected.\n", client_soc);
                    close(client_soc);
                    client_fds[i] = -1;
				} else {
					// Successful message received
                    printf("[Client %d]: %s\n", client_soc, message);
					
					// broadcast(message, client_fds);
					
                    // If client sent "quit", close the connection
                    if (strcmp(message, QUIT_MSG) == 0) {
                        printf("Closing connection to client %d.\n", client_soc);
                        close(client_soc);
                        client_fds[i] = -1;
                    }
				}
			}
		}

	}

	printf("quitting\n");
	cleanup(self, listenfd);
    
    return 0;
}

void broadcast(char* message, int* client_soc) {
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(client_soc[i] != -1) {
			send_message(client_soc[i], message);
		}
	}
}


void cleanup(struct sockaddr_in *self, int listenfd) {
	free(self);
    close(listenfd);
}