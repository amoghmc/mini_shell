// header guards to prevent double inclusion
#ifndef BUILTIN_H
#define BUILTIN_H

#include <readline/history.h>
#include "parse.h"

int isBuiltInCommand(char *command);
void executeBuiltInCommand(commandType *command, int type, HISTORY_STATE *historyState);

enum
BUILTIN_COMMANDS {
	EXIT, CD, HISTORY, JOBS, KILL, LEN, NO_SUCH_BUILTIN
};

#endif