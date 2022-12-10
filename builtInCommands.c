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

void executeBuiltInCommand(commandType *command, int type, HISTORY_STATE *history_state) {
	switch (type) {
		case CD:
			chdir(command->VarList[1]);
			break;
		case HISTORY:
//			Source: https://stackoverflow.com/questions/38792542/readline-h-history-usage-in-c
			for (int i = 0; i < history_state->length; i++) {
				printf(" %8s  %s", history_state->entries[i]->line, history_state->entries[i]->timestamp);
			}
			putchar('\n');
			break;
		default:
			printf("\nError! No such builtin command!");
	}
	free(history_state);
}