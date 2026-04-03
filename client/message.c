#include "socket.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const unsigned char PACKET_START = 254;
const unsigned char PACKET_END   = 255;
const int PACKET_SIZE        	 = 128;

int get_message(int socket, char* message) {
	unsigned char curr_char = '\0';
	int packet_unfinished = 1;
	int message_len = 0;

	// read until packet start
	while (curr_char != PACKET_START) {
		ssize_t n = read(socket, &curr_char, 1);
		if (n <= 0) {
			return -1;
		}
	}

	// while value read is not packet end, keep reading
	// into message
	while (packet_unfinished) {
		ssize_t n = read(socket, &curr_char, 1);
		if (n <= 0) {
			return -1;
		}
		// if we encounter a packet start char, malformed message
		if(curr_char == PACKET_START) {
			message[0] = '\0';
            return -1;
		}
		else if (curr_char == PACKET_END) {
			packet_unfinished = 0;
			continue;
		}
		else {
			if (message_len + 1 >= PACKET_SIZE) {
				message[0] = '\0';
				return -1;
			}
			message[message_len] = curr_char;
			message_len++;
		}
	}
	message[message_len] = '\0';
	return 0;
}

int write_all(int fd, const void *buf, size_t len) {
    const unsigned char *p = buf;
    size_t total = 0;

    while (total < len) {
        ssize_t n = write(fd, p + total, len - total);
        if (n < 0) {
            if (errno == EINTR) continue; // interrupted, retry
            return -1;                    // error
        }
        total += (size_t)n;
    }

    return 0;
}

int send_message(int socket, char* message) {
    size_t len = 0;
    while (message[len] != '\0') len++; // compute string length

    // send start marker
    if (write_all(socket, &PACKET_START, 1) < 0) return -1;

    // send payload
    if (len > 0 && write_all(socket, message, len) < 0) return -1;

    // send end marker
    if (write_all(socket, &PACKET_END, 1) < 0) return -1;

    return 0; // success
}