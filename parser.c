/************************************************************************  
 *   parse.c  -  The parsing portion of my small shell  
 *   Syntax:     myshell command1 [< infile] [| command]* [> outfile] [&]
 ************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"

//	Takes in a string cmdline, and returns a pointer to a struct parseInfo.
//	The members of parseInfo can be seen in parse.h.  Commands are always stored
//	in the array CommArray. This can accommodate multiple pipes. In commandType,
//	command is the executable name and VarList is the argv to pass to the program.
//	cmdline can end either with '\n' or '\0'.

parseInfo *parse(char *cmdline) {
	if (strpbrk(cmdline, "\t")) {
		return NULL;
	}

	if (strlen(cmdline) < 1)
		return NULL;

//	initialize parseInfo struct
	parseInfo *Result = init_info(Result);

	if (cmdline[strlen(cmdline) - 1] == '&') {
		Result->boolBackground = 1;
	}

	int pipe_delims, space_delims = 0;
//	splits string separated by pipe delimiter
	char **res_pipe = split_string(cmdline, &pipe_delims, "|");

	if (pipe_delims > PIPE_MAX_NUM) {
		error_check(Result, res_pipe, NULL, 2);
		return NULL;
	}
	int i;

//	for each sub-command separated by pipe delimiter
	for (i = 0; i < pipe_delims; i++) {
//		split sub-command separated by space delimiter
		char **res_space = split_string(res_pipe[i], &space_delims, " ");

		if (space_delims > MAX_VAR_NUM) {
			error_check(Result, res_pipe, res_space, 3);
			return NULL;
		}

		parse_command(Result, res_space, space_delims, i);
		free(res_space);
	}
//	store total # of pipes
	Result->pipeNum = i;
	free(res_pipe);

	return Result;
}

parseInfo *init_info(parseInfo *info) {
	printf("init_info: initializing parseInfo\n");
	info = malloc(sizeof(parseInfo));
	info->boolBackground = 0;
	return info;
}

void parse_command(parseInfo *Result, char **res_space, int space_delims, int i) {
	printf("parse_command: parsing a single command\n");

//	for each sub-command separated by space delimiter
	int j = 0;
	while (j < space_delims) {
//		store individual args of subcommand in var list in struct command type
		Result->CommArray[i].VarList[j] = res_space[j];
//		TODO
		if (strcmp(res_space[j], ">") == 0) {
			stpcpy(res_space[j + 1], Result->CommArray[i].outFile);
			Result->CommArray[i].boolOutfile = j;
			j++;
		}
		else if (strcmp(res_space[j], "<") == 0) {
			stpcpy(res_space[j + 1], Result->CommArray[i].inFile);
			Result->CommArray[i].boolInfile = j;
			j++;
		}
		j++;
	}
	Result->CommArray[i].VarList[space_delims] = NULL;

//	store name of subcommand
	Result->CommArray[i].command = res_space[0];
//	store total # of args of subcommand + 1 more for NULL
	Result->CommArray[i].VarNum = space_delims + 1;

}

void print_info(parseInfo *info) {
	printf("print_info: printing info about parseInfo struct\n");

//	for each command separated by pipe
	for (int i = 0; i < info->pipeNum; i++) {
		printf("Command[%d]: %s\n", i, info->CommArray[i].command);
		for (int k = 0; k < info->CommArray[i].VarNum; k++) {
			printf("Arg[%d]: %s\n", k, info->CommArray[i].VarList[k]);
		}
	}
	if (info->boolBackground) {
		printf("Background: Yes\n");
	} else {
		printf("Background: No\n");
	}

}

void free_info(parseInfo *info) {
	printf("\nfree_info: freeing memory associated to parseInfo struct\n");
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
			printf("\nError: Memory allocation failed!");
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

// prints appropriate error message when called
void error_check(parseInfo *info, char **res_pipe, char **res_space, int type) {
	switch (type) {
		case 1:
			printf("\nError: command too big to parse!");
			break;
		case 2:
			printf("\nError: too many pipes to parse!");
			free(info);
			free(res_pipe);
			break;
		case 3:
			printf("\nError: too many arguments for a single command to parse!");
			free(info);
			free(res_pipe);
			free(res_space);
			break;
		default:
			printf("\nError!");
			break;
	}
	printf("\n");
}