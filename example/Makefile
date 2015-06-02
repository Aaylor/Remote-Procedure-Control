
CC		= gcc
SOURCE	= call_client.c
OBJECT	= $(SOURCE:.c=.o)
TARGETS	= call_client

OCAMLC  	 = ocamlc
OCAML_SOURCE = callClient.ml
OCAML_TARGET = call_client_ml


.PHONY: all
all: $(TARGETS) $(OCAML_TARGET)


$(TARGETS): $(OBJECT)
	$(CC) -o $@ $^

$(OCAML_TARGET):
	$(OCAMLC) -o $@ unix.cma $(OCAML_SOURCE)


%.o: %.c
	$(CC) -o $@ -c $<



.PHONY: clean
clean:
	rm -f $(OBJECT) $(TARGETS) *.cm*
