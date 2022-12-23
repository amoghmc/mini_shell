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

void executeCommand(int in, int out, commandType *input_command, parseInfo *result);
int MAX_PATH = 1024;
job* first_job = NULL;
char input_copy[MAX_COM_SIZE];

//	Readline template source: https://en.wikipedia.org/wiki/GNU_Readline
int main() {
	// Configure readline to disable tab completion
	rl_bind_key('\t', rl_insert);
	using_history();

	while (1) {
		// Display prompt and read input
		char *buffer = print_prompt();
		char *input = readline(buffer);
		check_and_free(buffer)

		// Check for EOF.
		if (!input)
			break;

//		make a copy of command for use in jobs
		strcpy(input_copy, input);

		parseInfo *result = parse(input);
		if (result == NULL) {
			goto free;
		}

		//	add input to readline history.
		add_history(input_copy);
		time_t t;
		time(&t);
		add_history_time(ctime(&t));

		commandType *input_command = &result->CommArray[0];

//		execute builtin command in parent process
		int commType = isBuiltInCommand(input_command->command);
		if (commType == EXIT) {
			free_info(result);
			free_jobs(first_job);
			check_and_free(input)
			exit(0);
		} else if (commType != NO_SUCH_BUILTIN) {
			executeBuiltInCommand(input_command, commType, history_get_history_state(), first_job);
		} else {
//			create a child process for each | or & separated command
			int in, pipe_file_descriptor[2];

//			The first process should get its input from the original file descriptor 0
			in = STDIN_FILENO;
			for (int i = 0; i < result->pipeNum - 1; i++) {
//				Source: https://stackoverflow.com/questions/8082932/connecting-n-commands-with-pipes-in-a-shell
				pipe(pipe_file_descriptor);
				executeCommand(in, pipe_file_descriptor[1], &result->CommArray[i], result);

				/* No need for the write end of the pipe, the child will write here.  */
				close(pipe_file_descriptor[1]);
				/* Keep the read end of the pipe, the next child will read from there.  */
				in = pipe_file_descriptor[0];
			}
			executeCommand(in, STDOUT_FILENO, &result->CommArray[result->pipeNum - 1], result);
		}
		// Free buffer that was allocated by readline
		free:
		free_info(result);
		check_and_free(input)
	}
	return 0;
}

void executeCommand(int in, int out, commandType *input_command, parseInfo *result) {
	pid_t childPid;
	int status;
	childPid = fork();
//	inside child...
	if (childPid == 0) {
		if (input_command->boolInfile || input_command->boolOutfile) {
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
		} else if (!result->boolBackground){
			if (in != STDIN_FILENO) {
				dup2(in, STDIN_FILENO);
				close(in);
			}
			if (out != STDOUT_FILENO) {
				dup2(out, STDOUT_FILENO);
				close(out);
			}
		}
		execvp(input_command->command, input_command->VarList);
		perror("Error! execvp");
		free_info(result);
		exit(1);
	}
//	inside parent...
	else if (childPid > 0) {
		if (result->boolBackground) {
//			record in list of background jobs
			add_job(&first_job, childPid, input_copy);
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