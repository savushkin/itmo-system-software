CC = gcc

CFLAGS = -c -Wall
SRC = lab4.c
OBJ = $(SRC:.c=.o)
EXE = wc

all: $(OBJ) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
