/************************************************************************  
 *   parse.c  -  The parsing portion of the shell
 ************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "parse.h"

char *trim_whitespace(char *str);

void convert_tabs(char *str);

int countString(const char *haystack, const char *needle);

void init_sub_command(commandType *result, char *cmd, int *countIn, int *countOut);

//	Takes in a string cmdline, and returns a pointer to a struct parseInfo.
//	The members of parseInfo can be seen in parse.h.  Commands are always stored
//	in the array CommArray. This can accommodate multiple pipes. In commandType,
//	command is the executable name and VarList is the argv to pass to the program.
//	cmdline can end either with '\n' or '\0'.

parseInfo *parse(char *cmdline) {
//	get a substring with leading/trailing whitespace removed
	cmdline = trim_whitespace(cmdline);
//	check if input is null
	if (strlen(cmdline) < 1) {
		return NULL;
	}

//	ensure either & or | exists but not both
	if (strpbrk(cmdline, "&") && strpbrk(cmdline, "|")) {
		error_check(NULL, NULL, NULL, 4);
		return NULL;
	}

	//	ensure either one of (>, <) or | exists but not both
	if ((strpbrk(cmdline, "<") && strpbrk(cmdline, "|")) || (strpbrk(cmdline, ">") && strpbrk(cmdline, "|"))) {
		error_check(NULL, NULL, NULL, 6);
		return NULL;
	}

//	convert tabs into spaces
	convert_tabs(cmdline);

//	initialize parseInfo struct
	parseInfo *Result = NULL;
	Result = init_info(Result);

	char delim[] = "|";
	if (strpbrk(cmdline, "&")) {
		Result->boolBackground = true;
		strcpy(delim, "&");
	}

	int pipe_delims, space_delims = 0;

//	splits string separated by pipe delimiter
	char **result_pipe = split_string(cmdline, &pipe_delims, delim);

	if (pipe_delims > PIPE_MAX_NUM) {
		error_check(Result, result_pipe, NULL, 2);
		return NULL;
	}

	if (pipe_delims > 0 && (strcmp(delim, "&") == 0)) {
		error_check(Result, result_pipe, NULL, 7);
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

		if (space_delims > MAX_VAR_NUM) {
			error_check(Result, result_pipe, result_space, 3);
			check_and_free(cmd_copy)
			return NULL;
		}

		if (result_space[0] == NULL) {
			error_check(Result, result_pipe, result_space, -1);
			check_and_free(cmd_copy)
			return NULL;
		}

		int status = parse_command(&Result->CommArray[i], cmd_copy, result_space, space_delims);
		if (status) {
			error_check(Result, result_pipe, result_space, status);
//			check_and_free(cmd_copy)
			return NULL;
		}
		free_and_null(result_space)
	}
//	store total # of pipes
	Result->pipeNum = i;
	free_and_null(result_pipe)
	return Result;
}

parseInfo *init_info(parseInfo *info) {
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

void init_sub_command(commandType *result, char *cmd, int *countIn, int *countOut) {
	result->boolInfile = false;
	result->boolOutfile = false;
	if (strstr(cmd, ">") != NULL) {
		result->boolOutfile = true;
		*countOut = countString(cmd, ">");
	}
	if (strstr(cmd, "<") != NULL) {
		result->boolInfile = true;
		*countIn = countString(cmd, "<");
	}
	free_and_null(cmd)
}

int parse_command(commandType *result, char *cmd, char **res_space, int space_delims) {
	int countOut = 0, countIn = 0;
	init_sub_command(result, cmd, &countIn, &countOut);

	int j = 0;
	int displacement = 0;
//	both are not present
	if ((!result->boolOutfile) && (!result->boolInfile)) {
		for (int i = 0; i < space_delims; i++) {
			result->VarList[i] = strdup(res_space[i]);
		}
	}
//	if either operator is present
	else if ((result->boolOutfile) || (result->boolInfile)) {
//	ensure only 1 redirect is present in either direction
		if (countOut > 1 || countIn > 1) {
			return 5;
		}
		while (j < space_delims) {
			if (strcmp(res_space[j], ">") == 0) {
				//			if no output file exists return error 1
				if (res_space[j + 1] == NULL) {
					for (int k = 0; k < space_delims; k++) {
						check_and_free(result->VarList[k])
					}
					check_and_free(result->inFile)
					check_and_free(result->outFile)
					return 1;
				}
				result->outFile = strdup(res_space[j + 1]);
				result->boolOutfile = j;
				j++;
				displacement -= 2;
			} else if (strcmp(res_space[j], "<") == 0) {
				check_and_free(result->inFile)
				//			if no input file exists return error 1
				if (res_space[j + 1] == NULL) {
					for (int k = 0; k < space_delims; k++) {
						check_and_free(result->VarList[k])
					}
					check_and_free(result->inFile)
					check_and_free(result->outFile)
					return 1;
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
	}
//	end arr with NULL for execvp
	result->VarList[space_delims + displacement] = NULL;
//	store name of subcommand
	result->command = strdup(res_space[0]);
//	store total # of args of subcommand + 1 more for NULL
	result->VarNum = space_delims + displacement + 1;
	return 0;
}

void free_info(parseInfo *info) {
	if (info != NULL) {
		for (int i = 0; i < info->pipeNum; i++) {
			for (int j = 0; j < info->CommArray[i].VarNum; j++) {
				check_and_free(info->CommArray[i].VarList[j])
			}
			check_and_free(info->CommArray[i].inFile)
			check_and_free(info->CommArray[i].outFile)
			check_and_free(info->CommArray[i].command)
		}
		free_and_null(info)
	}
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
			printf("\nError: Memory allocation failed!\n");
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
			printf("\nError: STDIN/STDOUT is NULL!");
			break;
		case 2:
			printf("\nError: too many pipes to parse!");
			break;
		case 3:
			printf("\nError: too many arguments for a single command to parse!");
			break;
		case 4:
			printf("\nError: can't use jobs with pipes");
			break;
		case 5:
			printf("\nError: can have only one redirect in either direction!");
			break;
		case 6:
			printf("\nError: can't use I/O redirection with pipes");
			break;
		case 7:
			printf("\nError: can execute only one background job at a time");
			break;
		default:
			printf("\nError: Invalid command");
			break;
	}
	check_and_free(info)
	check_and_free(res_pipe)
	check_and_free(res_space)
	printf("\n");
}

//	Source: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
char *trim_whitespace(char *str) {
	char *end;

	// Trim leading space
	while (isspace((unsigned char) *str)) str++;

	if (*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char) *end)) end--;

	// Write new null terminator character
	end[1] = '\0';

	return str;
}

void convert_tabs(char *str) {
	for (int i = 0; i < strlen(str); i++) {
		if (str[i] == '\t') {
			str[i] = ' ';
		}
	}
}

int countString(const char *haystack, const char *needle) {
	int count = 0;
	const char *tmp = haystack;
	tmp = strstr(tmp, needle);
	while (tmp) {
		count++;
		tmp++;
		tmp = strstr(tmp, needle);
	}
	return count;
}