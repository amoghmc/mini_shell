// header guards to prevent double inclusion
#ifndef BUILTIN_H
#define BUILTIN_H

#include "parse.h"

int isBuiltInCommand(char *command);
void executeBuiltInCommand(commandType *command, int type);

extern char * builtInArray[];

#endif