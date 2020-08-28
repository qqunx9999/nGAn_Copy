all : client server
.PHONY : all
client : client.c transfer.h
	gcc -Wall -O2 client.c -o client
server : server.c transfer.h
	gcc -Wall -O2 server.c -o server
