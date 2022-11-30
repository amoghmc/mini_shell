#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "builtIn.h"


enum
BUILTIN_COMMANDS {
	NO_SUCH_BUILTIN = 0, EXIT, CD, HISTORY, JOBS, LEN
};

char *builtInArray[LEN - 1] = {
		"exit",
		"cd",
		"history",
		"jobs"
};

int isBuiltInCommand(char *command) {
	for (int i = 0; i < LEN - 1; i++) {
		if (strncmp(command, builtInArray[i], strlen(builtInArray[i])) == 0) {
			return i;
		}
	}
	return NO_SUCH_BUILTIN;
}

void executeBuiltInCommand(commandType* command, int type) {
	if (type == EXIT) {
		exit(0);
	}
	else if (type == CD) {
		chdir(command->VarList[1]);
		printf("asdasd");
		return;
	}
}