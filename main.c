#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "parser.c"


void print_prompt();

int isBuiltInCommand(char *command);

void executeBuiltInCommand(char *command);

void executeCommand(char *command, char *VarList[]);

enum
BUILTIN_COMMANDS {
	NO_SUCH_BUILTIN = 0, EXIT, JOBS
};
int MAX_PATH = 1024;

//	Readline template source: https://en.wikipedia.org/wiki/GNU_Readline
int main() {
	// Configure readline to auto-complete paths when the tab key is hit.
	rl_bind_key('\t', rl_complete);

	while (1) {
		int childPid;

		// Display prompt and read input
		print_prompt();

		char *input = readline("");

		// Check for EOF.
		if (!input)
			break;

		parseInfo *result = parse(input);

		if (result == NULL) {
			printf("GOING TO END\n");
			goto end;
		}

		print_info(result);


		// Add input to readline history.
		add_history(input);

		struct commandType* input_command = result->CommArray;

		// Do stuff...
		if (isBuiltInCommand(input_command->command)) {
			executeBuiltInCommand(input_command->command);
		}
		else {
			childPid = fork();
			if (childPid == 0){
				printf("executing\n");
				//calls execvp
				executeCommand(input_command->command, input_command->VarList);
//				_exit(0);
			}
			else {
//				if (isBackgroundJob(cmd)){
////					record in list of background jobs
//				} else {

					waitpid(childPid, 0, 0);
//				}
			}
		}

		// Free buffer that was allocated by readline
		end:
		free(input);
		free_info(result);
	}
	return 0;
}


void print_prompt() {
	char *cwd, *host = NULL;
	size_t allocSize = sizeof(char) * MAX_PATH;
	cwd = (char *) malloc(allocSize);

//	Source: https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux
//	Gets user id
	printf("%s@", getlogin());

//	Source: https://stackoverflow.com/questions/504810/how-do-i-find-the-current-machines-full-hostname-in-c-hostname-and-domain-info
//	Gets hostname of machine
	host = (char *) malloc(allocSize);
	gethostname(host, allocSize);
	printf("%s:", host);

//	Source: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
//	Gets current path of user
	getcwd(cwd, allocSize);
	if (getcwd(cwd, allocSize) != NULL) {
		printf("%s$ ", cwd);
	} else {
		perror("getcwd() error");
	}

	free(cwd);
	free(host);
}

int isBuiltInCommand(char *command) {

	if (strncmp(command, "exit", strlen("exit")) == 0) {
		return EXIT;
	}
	return NO_SUCH_BUILTIN;
}

void executeBuiltInCommand(char *cmd) {
	exit(0);
}

void executeCommand(char *command, char *VarList[]) {
	execvp(command, VarList);
}