
CC		= gcc
SOURCE	= call_client.c
OBJECT	= $(SOURCE:.c=.o)
TARGETS	= call_client


.PHONY: all
all: call_client


$(TARGETS): $(OBJECT)
	$(CC) -o $@ $^


%.o: %.c
	$(CC) -o $@ -c $<


.PHONY: clean
clean:
	rm -f $(OBJECT) $(TARGETS)
