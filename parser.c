/************************************************************************  
 *   parse.c  -  The parsing portion of my small shell  
 *   Syntax:     myshell command1 [< infile] [| command]* [> outfile] [&]
 ************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"

/*
 	* parse - parse a new line
 	* Accepts: nothing
 	* Returns: parse information structure
*/

#define MAXLINE 81

char **split_string(char *cmdline, int *n_delims, char *delim);
void error_check(parseInfo* info, char** res_pipe, char** res_space, int type);

parseInfo * init_info(parseInfo *info) {
	printf("init_info: initializing parseInfo\n");
	info = malloc(sizeof(parseInfo));
	info->boolBackground = 0;
	info->boolInfile = 0;
	info->boolOutfile = 0;
	return info;
}


void parse_command(char *command, struct commandType *comm) {
	printf("parse_command: parsing a single command\n");
}


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

//	check if len of command exceeds MAXLINE
	if ((strlen(cmdline) + 1 > MAXLINE)) {
        error_check(NULL, NULL, NULL, 1);
		return NULL;
	}
//	initialize parseInfo struct
	parseInfo *Result = init_info(Result);

	if(cmdline[strlen(cmdline)-1] == '&') {
		Result->boolBackground = 1;
	}

	int delims_pipes, delims_spaces = 0;
//	splits string separated by pipe delimiter
	char **res_pipe = split_string(cmdline, &delims_pipes, "|");

	if (delims_pipes > PIPE_MAX_NUM) {
        error_check(Result, res_pipe, NULL, 2);
		return NULL;
	}
	int i;

//	for each sub-command separated by pipe delimiter
	for (i = 0; i < delims_pipes; i++) {
//		split sub-command separated by space delimiter
		char **res_space = split_string(res_pipe[i], &delims_spaces, " ");

		if (delims_spaces > MAX_VAR_NUM) {
            error_check(Result, res_pipe, res_space, 3);
			return NULL;
		}

//	for each sub-command separated by space delimiter
		for (int j = 0; j < delims_spaces; j++) {
//			store individual args of subcommand in var list in struct command type
			Result->CommArray[i].VarList[j] = res_space[j];
		}
		Result->CommArray[i].VarList[delims_spaces] = NULL;

//		store name of subcommand
		Result->CommArray[i].command = res_space[0];
//		store total # of args of subcommand + 1 more for NULL
		Result->CommArray[i].VarNum = delims_spaces + 1;
		free(res_space);
	}
//	store total # of pipes
	Result->pipeNum = i;
	free(res_pipe);

	parse_command(cmdline, &Result->CommArray[Result->pipeNum]);
	return Result;
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
	}
	else {
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
void error_check(parseInfo* info, char** res_pipe, char** res_space, int type) {
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