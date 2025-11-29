
CC     = gcc
CFLAGS = -Wall -Wextra -O2

SRC    = $(wildcard src/*.c) $(wildcard utils/*c)
OBJ    = $(SRC:.c=.o)
BIN    = server

all: server client

server: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

client: client.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(BIN) client
