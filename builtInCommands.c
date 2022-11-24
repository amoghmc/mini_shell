#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "builtIn.h"

int isBuiltInCommand(char *command) {

	if (strncmp(command, "exit", strlen("exit")) == 0) {
		return EXIT;
	}
	return NO_SUCH_BUILTIN;
}

void executeBuiltInCommand(char *command) {
	exit(0);
}