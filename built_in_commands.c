//
// Created by amogh on 23/11/22.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int isBuiltInCommand(char *command);
void executeBuiltInCommand(char *command);

enum
BUILTIN_COMMANDS {
	NO_SUCH_BUILTIN = 0, EXIT, JOBS, CD, HISTORY
};

int isBuiltInCommand(char *command) {

	if (strncmp(command, "exit", strlen("exit")) == 0) {
		return EXIT;
	}
	return NO_SUCH_BUILTIN;
}

void executeBuiltInCommand(char *command) {
	exit(0);
}