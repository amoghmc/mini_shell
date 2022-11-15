/************************************************************************  
 *   parse.c  -  The parsing portion of my small shell  
 *   Syntax:     myshell command1 [< infile] [| command]* [> outfile] [&]
 ************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parse.h"

/* parse - parse a new line 
 *
 * Accepts: nothing
 * Returns: parse information structure
 */

#define MAXLINE 81

char **split_string(char *cmdline, int *n_delims, char *delim);

void init_info(parseInfo *p) {
	printf("init_info: initializing parseInfo\n");
}


void parse_command(char *command, struct commandType *comm) {
	printf("\nparse_command: parsing a single command\n");
}


//	Takes in a string cmdline, and returns a pointer to a struct parseInfo.
//	The members of parseInfo can be seen in parse.h.  Commands are always stored
//	in the array CommArray. This can accommodate multiple pipes. In commandType,
//	command is the executable name and VarList is the argv to pass to the program.
//	cmdline can end either with '\n' or '\0'.

parseInfo *parse(char *cmdline) {
	parseInfo *Result;

	if (strlen(cmdline) + 1 > MAXLINE) {
		printf("Error: command too big to parse!");
		exit(1);
	}

	Result = malloc(sizeof(parseInfo));
	init_info(Result);

	int delims_pipes, delims_spaces = 0;
//	splits string separated by pipes
	char **res_pipe = split_string(cmdline, &delims_pipes, "|");

	if (delims_pipes > PIPE_MAX_NUM) {
		printf("Error: too many pipes to parse!");
		exit(1);
	}
	int i;

//	for each command separated by pipe
	for (i = 0; i < delims_pipes; i++) {
//		split sub-command separated by spaces
		char **res_space = split_string(res_pipe[i], &delims_spaces, " ");

		if (delims_spaces > MAX_VAR_NUM) {
			printf("Error: too many arguments for a single command to parse!");
			exit(1);
		}
		for (int j = 0; j < delims_spaces; j++) {
			Result->CommArray[i].VarList[j] = res_space[j + 1];
		}

		Result->CommArray[i].command = res_space[0];
		Result->CommArray[i].VarNum = delims_spaces - 1;
		free(res_space);
	}
	Result->pipeNum = i;
	free(res_pipe);

	parse_command(cmdline, &Result->CommArray[Result->pipeNum]);
	return Result;
}

void print_info(parseInfo *info) {
	printf("print_info: printing info about parseInfo struct\n");

//	for each command separated by pipe
	for (int i = 0; i < info->pipeNum; i++) {
		printf("Command: %s\n", info->CommArray[i].command);
		for (int k = 0; k < info->CommArray[i].VarNum; k++) {
			printf("Arg[%d]: %s\n", k, info->CommArray[i].VarList[k]);
		}
	}
}

void free_info(parseInfo *info) {
	printf("free_info: freeing memory associated to parseInfo struct\n");
	free(info);
}

// Source: https://stackoverflow.com/questions/11198604/c-split-string-into-an-array-of-strings
char **split_string(char *cmdline, int *n_delim, char *delim) {
	char **res = NULL;
	char *delim_space = strtok(cmdline, delim);
	*n_delim = 0;

/* split string and append tokens to 'res' */
	while (delim_space) {
		res = realloc(res, sizeof(char *) * ++(*n_delim));
		if (res == NULL) {
			printf("Error: Memory allocation failed!");
			exit(-1); /* memory allocation failed */
		}
		res[*n_delim - 1] = delim_space;
		delim_space = strtok(NULL, delim);
	}

/* reallocate one extra element for the last NULL */
	res = realloc(res, sizeof(char *) * (*(n_delim) + 1));
	res[*n_delim] = NULL;
	free(delim_space);
	return res;
}
