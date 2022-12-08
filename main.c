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

int MAX_PATH = 1024;

//	Readline template source: https://en.wikipedia.org/wiki/GNU_Readline
int main() {
	// Configure readline to auto-complete paths when the tab key is hit.
//	rl_bind_key('\t', rl_complete);

	using_history();
	while (1) {
		int childPid, status;

		// Display prompt and read input
		char *buffer = print_prompt();
		char *input = readline(buffer);
		check_and_free(buffer)

//		add input to readline history.
		add_history(input);
		time_t t;
		time(&t);
		add_history_time(ctime(&t));

		// Check for EOF.
		if (!input)
			break;

		parseInfo *result = parse(input);
		if (result == NULL) {
			goto free;
		}
		print_info(result);

		commandType *input_command = &result->CommArray[0];

//		execute builtin command in parent process
		int commType = isBuiltInCommand(input_command->command);
		if (commType != NO_SUCH_BUILTIN) {
			executeBuiltInCommand(input_command, commType, history_get_history_state());
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
//				todo how to run a daemon process?
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
		free:
		free_info(result);
		check_and_free(input)
	}
	return 0;
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