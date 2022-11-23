CFLAGS=-DUNIX -lreadline -lcurses
DEBUG=-g
#DEBUG=

all: shell

shell: main.c parser.c parse.h
	gcc $(DEBUG) main.c parser.c -o shell $(CFLAGS)
clean:
	rm -f shell *~
