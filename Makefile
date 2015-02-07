CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -D _POSIX_C_SOURCE=200112L -Werror -Wextra

SRC=src/
BIN=bin/

SOURCES=$(wildcard $(SRC)*.c)
OBJECTS=$(SOURCES:.c=.o)

TARGETS=client server

CLIENT_SOURCES=$(wildcard $(SRC)c_*.c)
CLIENT_OBJECTS=$(CLIENT_SOURCES:.c=.o)

SERVER_SOURCES=$(wildcard $(SRC)s_*.c)
SERVER_OBJECTS=$(SERVER_SOURCES:.c=.o)

.PHONY: all clean



all: $(TARGETS)

client: $(CLIENT_OBJECTS)
	$(CC) -o $(BIN)$@ $^

server: $(SERVER_OBJECTS)
	$(CC) -o $(BIN)$@ $^

$(OBJECTS): $(SOURCES)
	$(CC) -o $@ -c $< $(CFLAGS)


clean:
	rm -f $(OBJECTS)

