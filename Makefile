CC=gcc
CFLAGS=-Wall -Werror
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)

.PHONY: all clean

all:

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf $(OBJECTS)


