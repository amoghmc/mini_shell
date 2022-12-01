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

void executeBuiltInCommand(commandType *command, int type, historyType *history_command) {
	switch (type) {
		case CD:
			chdir(command->VarList[1]);
			break;
		case HISTORY:
//			Source: https://stackoverflow.com/questions/38792542/readline-h-history-usage-in-c
			for (int i = 0; i < history_command->historyState->length; i++) { /* output history list */
				printf(" %8s  %s\n", history_command->historyEntry[i]->line, history_command->historyEntry[i]->timestamp);
			}
			putchar('\n');
			break;
		default:
//			free_history(history_command);
			exit(0);
	}
}