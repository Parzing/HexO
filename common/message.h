#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#define PACKET_SIZE 128

extern const unsigned char PACKET_START;
extern const unsigned char PACKET_END;

int get_message(int socket, char* message);
int send_message(int socket, char* message);

#endif
