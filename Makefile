CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = server

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: server client


server: server.c
	$(CC) $(CFLAGS) server.c -o server	

client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -rf client server
