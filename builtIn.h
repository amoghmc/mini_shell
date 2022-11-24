int isBuiltInCommand(char *command);
void executeBuiltInCommand(char *command);

enum
BUILTIN_COMMANDS {
	NO_SUCH_BUILTIN = 0, EXIT, JOBS, CD, HISTORY
};
