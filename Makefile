UNAME = $(shell uname -s)

CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -D _POSIX_C_SOURCE=200112L -Werror -Wextra

#=-=-=-=-=-=-=-= RPC Variables =-=-=-=-=-=-=-=-=#

SRC=src/
BIN=bin/

SOURCES=$(wildcard $(SRC)*.c)
OBJECTS=$(SOURCES:.c=.o)

TARGETS=client server

CLIENT_SOURCES=$(wildcard $(SRC)c_*.c)
CLIENT_OBJECTS=$(CLIENT_SOURCES:.c=.o)

SERVER_SOURCES=$(wildcard $(SRC)s_*.c)
SERVER_OBJECTS=$(SERVER_SOURCES:.c=.o)

#=-=-=-=-=-= Librairies variables  =-=-=-=-=-=-=#

LIBNET=libs/libnet
LIBNET_DYN=net

#=-=-=-=-=-=-=-= Documentation =-=-=-=-=-=-=-=-=#

DOXYGEN=$(shell which doxygen)
DOXYGEN_FLAGS=
DOXYGEN_MAIN_CONF=doc/rpc.conf
DOC_FOLDER=doc/rpc



.PHONY: all libs doc doc-clean libs-clean clean

all: libs $(TARGETS)

libs:
	@echo "=-=-= making libnet =-=-=-=-=-=-=-=-=-="
	@make dyn -C $(LIBNET)
	@echo "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="

client: $(CLIENT_OBJECTS)
	$(CC) -L$(LIBNET) -o $(BIN)$@ $^ -l$(LIBNET_DYN)

server: $(SERVER_OBJECTS)
	$(CC) -L$(LIBNET) -o $(BIN)$@ $^ -l$(LIBNET_DYN)

$(OBJECTS): $(SOURCES)
	$(CC) -I$(LIBNET) -o $@ -c $< $(CFLAGS)

doc:
	$(DOXYGEN) $(DOXYGEN_FLAGS) $(DOXYGEN_MAIN_CONF) > /dev/null

doc-clean:
	rm -rf $(DOC_FOLDER)

libs-clean:
	@make clean -C $(LIBNET)

clean: doc-clean libs-clean
	rm -f $(OBJECTS) $(BIN)client $(BIN)server

