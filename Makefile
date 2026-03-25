FLAGS = -Wall -o 

all: server

server: server.o 
	gcc $(FLAGS) server server.c