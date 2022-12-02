/************************************************************************  
 *   parse.c  -  The parsing portion of my small shell  
 *   Syntax:     myshell command1 [< infile] [| command]* [> outfile] [&]
 ************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parse.h"
#define check_and_free(x) if (x != NULL) { free_and_null(x) }

//	Takes in a string cmdline, and returns a pointer to a struct parseInfo.
//	The members of parseInfo can be seen in parse.h.  Commands are always stored
//	in the array CommArray. This can accommodate multiple pipes. In commandType,
//	command is the executable name and VarList is the argv to pass to the program.
//	cmdline can end either with '\n' or '\0'.

parseInfo *parse(char *cmdline) {
	if (strpbrk(cmdline, "\t") || strlen(cmdline) < 1) {
		return NULL;
	}

//	initialize parseInfo struct
	parseInfo *Result = NULL;
	Result = init_info(Result);

	if (cmdline[strlen(cmdline) - 1] == '&') {
		Result->boolBackground = 1;
	}

	int pipe_delims, space_delims = 0;
//	splits string separated by pipe delimiter
	char **result_pipe = split_string(cmdline, &pipe_delims, "|");

	if (pipe_delims > PIPE_MAX_NUM) {
		error_check(Result, result_pipe, NULL, 2);
		return NULL;
	}
	int i;

//	for each sub-command separated by pipe delimiter
	for (i = 0; i < pipe_delims; i++) {
//		split sub-command separated by space delimiter

		if (result_pipe[i] == NULL) {
			error_check(Result, result_pipe, NULL, 2);
			return NULL;
		}
		char *cmd_copy = strdup(result_pipe[i]);
		char **result_space = split_string(result_pipe[i], &space_delims, " ");

		if (space_delims > MAX_VAR_NUM || result_space[0] == NULL) {
			error_check(Result, result_pipe, result_space, 3);
			check_and_free(cmd_copy)
			return NULL;
		}

		parse_command(&Result->CommArray[i], cmd_copy, result_space, space_delims);
		free_and_null(result_space)
	}
//	store total # of pipes
	Result->pipeNum = i;
	free_and_null(result_pipe)
	return Result;
}

parseInfo *init_info(parseInfo *info) {
	printf("init_info: initializing parseInfo\n");
	info = malloc(sizeof(parseInfo));

	for (int i = 0; i < PIPE_MAX_NUM; i++) {
		commandType command;
		command.inFile = NULL;
		command.outFile = NULL;
		command.command = NULL;
		for (int j = 0; j < MAX_VAR_NUM; j++) {
			command.VarList[j] = NULL;
		}
		info->CommArray[i] = command;
	}
	info->boolBackground = false;
	return info;
}

void parse_command(commandType *result, char *cmd, char **res_space, int space_delims) {
	printf("parse_command: parsing a single command\n");

//	for each sub-command separated by space delimiter
	int j = 0;
	result->boolInfile = false;
	result->boolOutfile = false;
	if (strstr(cmd, ">") != NULL) {
		result->boolOutfile = true;
	}
	if (strstr(cmd, "<") != NULL) {
		result->boolInfile = true;
	}
	free(cmd);
	cmd = NULL;

//	both are not present
	if ((!result->boolOutfile) && (!result->boolInfile)) {
		for (int i = 0; i < space_delims; i++) {
			result->VarList[i] = strdup(res_space[i]);
		}
//		end arr with NULL for execvp
		result->VarList[space_delims] = NULL;
//		store name of subcommand
		result->command = strdup(res_space[0]);
//		store total # of args of subcommand + 1 more for NULL
		result->VarNum = space_delims + 1;
	}
//	< is present
	else if ((!result->boolOutfile) && (result->boolInfile)) {
		result->VarNum = 0;
		int displacement = 0;
		while (j < space_delims) {
			if (strcmp(res_space[j], "<") == 0) {
				if (result->inFile != NULL) {
					free_and_null(result->inFile)
				}
				result->inFile = strdup(res_space[j + 1]);
				result->boolInfile = j;
				j++;
				displacement -= 2;
			} else {
				result->VarList[j + displacement] = strdup(res_space[j]);
			}
			j++;
		}
//		end arr with NULL for execvp
		result->VarList[space_delims + displacement] = NULL;
//		store name of subcommand
		result->command = strdup(res_space[0]);
//		store total # of args of subcommand + 1 more for NULL
		result->VarNum = space_delims + displacement + 1;
	}
//	> is present
	else if ((result->boolOutfile) && (!result->boolInfile)) {
		result->VarNum = 0;
		int displacement = 0;
		while (j < space_delims) {
			if (strcmp(res_space[j], ">") == 0) {
				if (result->outFile != NULL) {
					free_and_null(result->outFile)
				}
				result->outFile = strdup(res_space[j + 1]);
				result->boolOutfile = j;
				j++;
				displacement -= 2;
			} else {
				result->VarList[j + displacement] = strdup(res_space[j]);
			}
			j++;
		}
//		end arr with NULL for execvp
		result->VarList[space_delims + displacement] = NULL;
//		store name of subcommand
		result->command = strdup(res_space[0]);
//		store total # of args of subcommand + 1 more for NULL
		result->VarNum = space_delims + displacement + 1;
	}
}

void print_info(parseInfo *info) {
	printf("print_info: printing info about parseInfo struct\n");

//	for each command separated by pipe
	for (int i = 0; i < info->pipeNum; i++) {
		printf("Command[%d]: %s\n", i, info->CommArray[i].command);
		if (info->CommArray[i].boolInfile) {
			printf("Infile: %s\n", info->CommArray[i].inFile);
		}
		if (info->CommArray[i].boolOutfile) {
			printf("Outfile: %s\n", info->CommArray[i].outFile);
		}
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
	for (int i = 0; i < info->pipeNum; i++) {
		for (int j = 0; j < info->CommArray[i].VarNum; j++) {
			check_and_free(info->CommArray[i].VarList[j])
		}
		check_and_free(info->CommArray[i].inFile)
		check_and_free(info->CommArray[i].outFile)
		check_and_free(info->CommArray[i].command)
	}
	check_and_free(info)
}

// Source: https://stackoverflow.com/questions/11198604/c-split-string-into-an-array-of-strings
char **split_string(char *cmdline, int *n_delim, char *delim) {
	char **result = NULL;
	char *delim_space = strtok(cmdline, delim);
	*n_delim = 0;

/* split string and append tokens to 'result' */
	while (delim_space) {
		result = realloc(result, sizeof(char *) * ++(*n_delim));
		if (result == NULL) {
			printf("\nError: Memory allocation failed!");
			exit(-1); /* memory allocation failed */
		}
		result[*n_delim - 1] = delim_space;
		delim_space = strtok(NULL, delim);
	}

/* reallocate one extra element for the last NULL */
	result = realloc(result, sizeof(char *) * (*(n_delim) + 1));
	result[*n_delim] = NULL;
	free_and_null(delim_space)
	return result;
}

// prints appropriate error message when called
void error_check(parseInfo *info, char **res_pipe, char **res_space, int type) {
	switch (type) {
		case 1:
			printf("\nError: command too big to parse!");
			break;
		case 2:
			printf("\nError: too many pipes to parse!");
			break;
		case 3:
			printf("\nError: too many arguments for a single command to parse!");
			break;
		default:
			printf("\nError!");
			break;
	}
	check_and_free(info)
	check_and_free(res_pipe)
   	check_and_free(res_space)
	printf("\n");
}