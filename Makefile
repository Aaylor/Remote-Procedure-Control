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

DOXYGEN=$(shell which doxygen)
DOXYGEN_FLAGS=
DOXYGEN_MAIN_CONF=doc/rpc.conf
DOC_FOLDER=doc/rpc



.PHONY: all clean doc doc-clean

all: $(TARGETS)

client: $(CLIENT_OBJECTS)
	$(CC) -o $(BIN)$@ $^

server: $(SERVER_OBJECTS)
	$(CC) -o $(BIN)$@ $^

$(OBJECTS): $(SOURCES)
	$(CC) -o $@ -c $< $(CFLAGS)

doc:
	$(DOXYGEN) $(DOXYGEN_FLAGS) $(DOXYGEN_MAIN_CONF) > /dev/null

doc-clean:
	rm -rf $(DOC_FOLDER)

clean: doc-clean
	rm -f $(OBJECTS)

