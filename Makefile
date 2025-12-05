
CC     = gcc
CFLAGS = -Wall -Wextra -O2

SRC    = $(wildcard src/*.c) 
OBJ    = $(SRC:.c=.o)
BIN    = server

all: server client

server: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN) -g

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -g 

client: client.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(BIN) client
