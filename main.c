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

void executeCommand(commandType *input_command, parseInfo *result);

int MAX_PATH = 1024;

//	Readline template source: https://en.wikipedia.org/wiki/GNU_Readline
int main() {
	// Configure readline to disable tab completion
	rl_bind_key('\t', rl_insert);

	using_history();
	while (1) {
		pid_t childPid;
		int status;

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

		commandType *input_command = &result->CommArray[0];
//		job_info* jobs = NULL;
//		jobs = init_jobs(jobs);

//		execute builtin command in parent process
		int commType = isBuiltInCommand(input_command->command);
		if (commType == EXIT) {
			free_info(result);
			check_and_free(input)
			exit(0);
		} else if (commType != NO_SUCH_BUILTIN) {
			executeBuiltInCommand(input_command, commType, history_get_history_state());
		} else {
//			create a child process for each | or & separated command
			for (int i = 0; i < result->pipeNum; i++) {
				executeCommand(&result->CommArray[i], result);
			}
		}
		// Free buffer that was allocated by readline
		free:
		free_info(result);
		check_and_free(input)
	}
	return 0;
}

void executeCommand(commandType *input_command, parseInfo *result) {
	pid_t childPid;
	int status;
	int in, out;
	childPid = fork();
//	inside child...
	if (childPid == 0) {
		if (input_command->boolInfile) {
			in = open(input_command->inFile, O_RDONLY);
			dup2(in, STDIN_FILENO);
			close(in);
		}
		if (input_command->boolOutfile) {
			out = open(input_command->outFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			dup2(out, STDOUT_FILENO);
			close(out);
		}
//		if (in != STDIN_FILENO){
//			dup2(in, STDIN_FILENO);
//			close (in);
//		}
//		if (out != STDOUT_FILENO){
//			dup2(out, STDOUT_FILENO);
//			close(out);
//		}
		execvp(input_command->command, input_command->VarList);
		perror("Error! execvp");
		free_info(result);
		exit(1);
	}
//	inside parent...
	else if (childPid > 0) {
		if (result->boolBackground) {
////		record in list of background jobs
			waitpid(childPid, &status, WNOHANG);
		} else {
			waitpid(childPid, &status, NO_MATCH);
		}
	} else {
		perror("Error! fork");
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