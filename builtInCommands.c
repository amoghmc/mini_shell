#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "builtIn.h"


const char *builtInArray[LEN] = {
		"exit",
		"cd",
		"history",
		"jobs"
};

int isBuiltInCommand(char *command) {
	for (int i = EXIT; i < LEN; i++) {
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
		return;
	}
}