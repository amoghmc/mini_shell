#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "builtIn.h"
#include "parse.h"


char *print_prompt();

void executeCommand(char *command, char *VarList[]);
historyType *init_history(historyType *history_command);
void free_history(historyType *history_command);

int MAX_PATH = 1024;

//	Readline template source: https://en.wikipedia.org/wiki/GNU_Readline
int main() {
	// Configure readline to auto-complete paths when the tab key is hit.
	rl_bind_key('\t', rl_complete);

	while (1) {
		int childPid, status;

		// Display prompt and read input
		char *buffer = print_prompt();
		char *input = readline(buffer);
		if (buffer != NULL) {
			free_and_null(buffer)
		}

		// Check for EOF.
		if (!input)
			break;

		parseInfo *result = parse(input);
		if (result == NULL) {
			goto free;
		}
		print_info(result);
//		add input to readline history.
		add_history(input);

		commandType *input_command = &result->CommArray[0];

//		execute builtin command in parent process
		int commType = isBuiltInCommand(input_command->command);
//		if (commType == EXIT) {
//			exit(0);
//		}
		if (commType != NO_SUCH_BUILTIN) {
			historyType *history_command = NULL;
			history_command = init_history(history_command);
			executeBuiltInCommand(input_command, commType, history_command);
			free_history(history_command);
		} else {
//			create a child process to execute command
			childPid = fork();
			if (childPid == 0) {
//				calls execvp
				printf("Executing child process...\n\n");
				if (!input_command->boolInfile && !input_command->boolOutfile) {
					execvp(input_command->command, input_command->VarList);
				} else if (input_command->boolInfile) {
					int fd = open(input_command->inFile, O_RDONLY);
					dup2(fd, STDIN_FILENO);
					execvp(input_command->command, input_command->VarList);
				} else {
					int fd = open(input_command->outFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
					dup2(fd, STDOUT_FILENO);
					execvp(input_command->command, input_command->VarList);
				}
				printf("Failed to execute command!\n");
				free_info(result);
				exit(1);
			} else {
//				if (isBackgroundJob(cmd)){
////					record in list of background jobs
//				} else {

				waitpid(childPid, &status, 0);
				if (status != 0) {
					printf("Error! Child exited with error code %d\n", WEXITSTATUS(status));
				}
			}
		}
		// Free buffer that was allocated by readline
		free_info(result);
		free:
		if (input != NULL) {
			free_and_null(input)
		}
	}
	return 0;
}

historyType *init_history(historyType *history_command) {
	history_command = malloc(sizeof(historyType));
	history_command->historyState = history_get_history_state();
	history_command->historyEntry = history_list();
	return history_command;
}

void free_history(historyType *history_command) {
	for (int i = 0; i < history_command->historyState->length; i++) {
		if (history_command->historyEntry[i] != NULL) {
			free_history_entry(history_command->historyEntry[i]);
		}
	}
	putchar('\n');
	if (history_command->historyEntry != NULL) {
		free_and_null(history_command->historyEntry)
	}
	if (history_command->historyState != NULL) {
		free_and_null(history_command->historyState)
	}
	if (history_command != NULL) {
		free_and_null(history_command)
	}
}

char *print_prompt() {
	char *buffer;
	char *cwd, *host = NULL;
	size_t allocSize = sizeof(char) * MAX_PATH;
	cwd = (char *) malloc(allocSize);
	buffer = (char *) malloc(allocSize);

//	Source: https://stackoverflow.com/questions/504810/how-do-i-find-the-current-machines-full-hostname-in-c-hostname-and-domain-info
//	Gets hostname of machine
	host = (char *) malloc(allocSize);
	gethostname(host, allocSize);

//	Source: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
//	Gets current path of user
	getcwd(cwd, allocSize);

	snprintf(buffer, MAX_PATH, "%s@%s:%s$ ", getlogin(), host, cwd);
	free_and_null(cwd)
	free_and_null(host)
	return buffer;
}