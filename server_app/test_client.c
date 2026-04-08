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

int main() {
    int soc = connect_to_server(PORT, "127.0.0.1");
    if (soc < 0) {
        perror("connect_to_server");
        exit(1);
    }

    printf("Connected to server.\n");
    printf("> ");
    fflush(stdout);

    char send_buf[PACKET_SIZE];
    char recv_buf[PACKET_SIZE];
    
    fd_set all_fds;
    int max_fd = (soc > STDIN_FILENO) ? soc : STDIN_FILENO;

	while (1) {
        // Reset the set every time because select() modifies it
        FD_ZERO(&all_fds);
        FD_SET(STDIN_FILENO, &all_fds);
        FD_SET(soc, &all_fds);

        // select() waits until either stdin or the socket has data
        if (select(max_fd + 1, &all_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        // CASE 1: Data is available on the socket (Incoming message from server)
        if (FD_ISSET(soc, &all_fds)) {
            if (get_message(soc, recv_buf) < 0) {
                printf("\nServer disconnected.\n");
                break;
            }
            // Print response and refresh the prompt
            printf("\r$ %s\n> ", recv_buf);
            fflush(stdout);
        }

        // CASE 2: User typed something in the terminal
        if (FD_ISSET(STDIN_FILENO, &all_fds)) {
            if (fgets(send_buf, PACKET_SIZE, stdin) == NULL) {
                break;
            }

            // Clean up newline
            size_t len = strlen(send_buf);
            if (len > 0 && send_buf[len - 1] == '\n') {
                send_buf[len - 1] = '\0';
            }

            // Send the message
            if (send_message(soc, send_buf) < 0) {
                perror("send_message");
                break;
            }

            // Handle quitting
            if (strcmp(send_buf, QUITTING) == 0) {
                break;
            }
            
            printf("> ");
            fflush(stdout);
        }
    }

    close(soc);
    printf("Disconnected.\n");
    return 0;
}

void send_to_socket(int socket, char *message) {
	write(socket, &PACKET_START, 1);
	write(socket, message, strlen(message));
	write(socket, &PACKET_END, 1);
}