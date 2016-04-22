PROJS=server client
CC=gcc
LOADLIBES=-lsocket -lnsl -lpthread 
CFLAGS=-m64
OBJECT=processing.o

all: server client

server: processing.o server.c
	$(CC) $(CFLAFS) -o server $(LOADLIBES) server.c processing.o 
	@echo Server Done!

client:
	$(CC) $(CFLAGS) -o client $(LOADLIBES) client.c
	@echo Client Done!

processing.o: processing.c
	$(CC) -c processing.c 
