#ifndef LOG_C
#define LOG_C

#define CLIENT_LOG ("./client_app/logs.txt")
#define SERVER_LOG ("./server_app/logs.txt")

void log_message(char* message, char *path);
void clear_logs(char *path);

#endif