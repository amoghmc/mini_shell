CFLAGS=-DUNIX -lreadline -lcurses
DEBUG=-g
#DEBUG=

all: shell

shell: main.c parser.c parse.h builtInCommands.c builtIn.h
	gcc $(DEBUG) main.c parser.c builtInCommands.c -o shell $(CFLAGS)

clean:
	rm -f shell *~

target:
	./shell