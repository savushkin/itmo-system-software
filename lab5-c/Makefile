CC=gcc

CFLAGS=-m32 -pedantic -Wall -Wextra -std=gnu99
LDFLAGS=-lpthread -lrt

1CLIENTBIN=1client
1SERVERBIN=1server
2CLIENTBIN=2client
2SERVERBIN=2server

4TASKBIN=4task
5TASKBIN=5task

8CLIENTBIN=8client
8SERVERBIN=8server

9TASKBIN=9task

all: $(1CLIENTBIN) $(1SERVERBIN) $(2CLIENTBIN) $(2SERVERBIN) $(4TASKBIN) $(5TASKBIN) $(8CLIENTBIN) $(8SERVERBIN) $(8TASKBIN) $(9TASKBIN)

$(1CLIENTBIN): 1client.o
	gcc 1client.o -o 1client

$(1SERVERBIN): 1server.o
	gcc 1server.o -o 1server

$(2CLIENTBIN): 2client.o
	gcc 2client.o -o 2client

$(2SERVERBIN): 2server.o
	gcc 2server.o -o 2server

$(4TASKBIN): 4task.o
	gcc -lpthread -lrt -D_XOPEN_SOURCE=500 4task.o -o 4task

$(5TASKBIN): 5task.o
	gcc -lpthread -lrt -D_XOPEN_SOURCE=500 5task.o -o 5task

$(9SERVERBIN): 9task.o
	gcc 9task.o -o 9task

$(8CLIENTBIN): 8client.o
	gcc 8client.o -o 8client -lsocket

$(8SERVERBIN): 8server.o
	gcc 8server.o -o 8server -lsocket

.PHONY: clean
clean:
	rm -rf *.o $(1CLIENTBIN) $(1SERVERBIN) $(2CLIENTBIN) $(2SERVERBIN) $(4TASKBIN) $(5TASKBIN) $(8CLIENTBIN) $(8SERVERBIN) $(8TASKBIN) $(9TASKBIN)
