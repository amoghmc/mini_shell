// header guards to prevent double inclusion
#ifndef BUILTIN_H
#define BUILTIN_H

#include <readline/history.h>
#include "parse.h"

typedef struct {
	HISTORY_STATE *historyState;
	HIST_ENTRY **historyEntry;
} historyType;

int isBuiltInCommand(char *command);
void executeBuiltInCommand(commandType *command, int type, historyType *history_command);

enum
BUILTIN_COMMANDS {
	EXIT, CD, HISTORY, JOBS, LEN, NO_SUCH_BUILTIN
};

#endif