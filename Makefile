UNAME = $(shell uname -s)

CC=gcc
PYTHON=python

ifeq ($(DEBUG),1)
	CFLAGS=-Wall -pedantic -std=c99 -D _POSIX_C_SOURCE=200112L -Werror -Wextra \
		   -D DEBUGLOG
else
	CFLAGS=-Wall -pedantic -std=c99 -D _POSIX_C_SOURCE=200112L -Werror -Wextra
endif

#=-=-=-=-=-=-=-= RPC Variables =-=-=-=-=-=-=-=-=#

SRC=src/
BIN=bin/

SOURCES = $(wildcard $(SRC)*.c)
OBJECTS = $(SOURCES:.c=.o)

TARGETS = $(BIN)client $(BIN)server

GEN_SCRIPT = ext/pump.py
GENERATION = $(wildcard $(SRC)*.c.pump)
GENERATED  = $(GENERATION:.pump=)

UTIL_SOURCES   = $(wildcard $(SRC)u_*.c)
UTIL_OBJECTS   = $(UTIL_SOURCES:.c=.o)
UTIL_HEADERS   = $(UTIL_SOURCES:.c=.h)
UTIL_UNIT_TEST = tests/check_utils

CLIENT_SOURCES = $(wildcard $(SRC)c_*.c)
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)

# FIXME: find a way to allow generation everywhere
SERVER_SOURCES   = $(wildcard $(SRC)s_*.c) $(GENERATED)
SERVER_OBJECTS   = $(SERVER_SOURCES:.c=.o)
SERVER_UNIT_TEST = tests/check_server



#=-=-=-=-=-=-=-= Documentation =-=-=-=-=-=-=-=-=#

DOXYGEN=$(shell which doxygen)
DOXYGEN_FLAGS=
DOXYGEN_MAIN_CONF=doc/rpc.conf
DOC_FOLDER=doc/rpc


#=-=-=-=-=-=-=-=- Dependencies -=-=--=-=-=-=-=-=-=#

.PHONY: all doc doc-clean generate libs-clean clean tests

all: generate $(TARGETS)

generate:
	@ echo "Generating some files... (can take a while)"
	@ for pump_file in $(GENERATION); do				\
		printf "Generation of $$pump_file ";		\
		ocaml unix.cma ext/check_date.ml $${pump_file%.pump} $$pump_file; \
		if [ "$$?" = "0" ]; then \
			echo "✓ (already updated)"; \
		else \
			$(PYTHON) $(GEN_SCRIPT) $$pump_file;		\
			[ "$$?" = "0" ] && echo "✓" || echo "✗";	\
		fi; \
	done

$(BIN)client: $(CLIENT_OBJECTS) $(UTIL_OBJECTS)
	$(CC) -o $@ $^
	@ rm -f client
	@ ln -s $@ .

$(BIN)server: $(SERVER_OBJECTS) $(UTIL_OBJECTS)
	$(CC) -pthread -o $@ $^
	@ rm -f server
	@ ln -s $@ .

$(SRC)client.o: $(UTIL_HEADERS)
$(SRC)server.o: $(UTIL_HEADERS)

tests:
	cd src; $(CC) -c *.c -c -D UNIT_TEST
	$(CC) -Isrc -Llib -lcheck $(UTIL_UNIT_TEST).c -o \
		$(UTIL_UNIT_TEST).utest $(OBJECTS)
	$(CC) -Isrc -Llib -lcheck $(SERVER_UNIT_TEST).c \
		-o $(SERVER_UNIT_TEST).utest $(OBJECTS)
	@for t in tests/*.utest; do						\
		echo "\n~~~ $$t ~~~\n";						\
		DYLD_LIBRARY_PATH="$(LIBNET)" ./$$t;										\
		echo "\n~~~~~~~~~~~~~~~~~~\n";				\
	done
	@rm -rf src/*.o

%.o: %.c %.h
	$(CC) -o $@ -c $< $(CFLAGS)

doc:
	$(DOXYGEN) $(DOXYGEN_FLAGS) $(DOXYGEN_MAIN_CONF) > /dev/null

doc-clean:
	rm -rf $(DOC_FOLDER)

clean: doc-clean
	rm -f $(OBJECTS) $(BIN)client $(BIN)server tests/*.utest

