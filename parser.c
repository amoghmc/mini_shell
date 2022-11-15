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


void init_info(parseInfo *p) {
	printf("\ninit_info: initializing parseInfo\n");
}

void parse_command(char * command, struct commandType *comm) {
	printf("\nparse_command: parsing a single command\n");
}


//	Takes in a string cmdline, and returns a pointer to a struct parseInfo.
//	The members of parseInfo can be seen in parse.h.  Commands are always stored
//	in the array CommArray. This can accommodate multiple pipes. In commandType,
//	command is the executable name and VarList is the argv to pass to the program.
//	cmdline can end either with '\n' or '\0'.

parseInfo *parse (char *cmdline) {
	parseInfo *Result;
	char command[MAXLINE];

	Result = malloc(sizeof(parseInfo));
	init_info(Result);

	char ** res = NULL;
	char *  delim_space = strtok (cmdline, " ");
	int com_pos = 0, i;

/* split string and append tokens to 'res' */
	while (delim_space) {
		res = realloc (res, sizeof (char*) * ++com_pos);
		if (res == NULL) {
			printf("Error: Memory allocation failed!");
			exit(-1); /* memory allocation failed */
		}
		res[com_pos - 1] = delim_space;
		delim_space = strtok (NULL, " ");
	}

/* reallocate one extra element for the last NULL */
	res = realloc (res, sizeof (char*) * (com_pos + 1));
	res[com_pos] = NULL;


	for (i = 0; i < (com_pos); ++i) {
		Result->CommArray[0].VarList[i] = res[i + 1];
	}
	Result->CommArray[0].command = res[0];
	Result->CommArray[0].VarNum = com_pos - 1;


//	print_info(Result);
	free(res);
	free(delim_space);

	parse_command(cmdline, &Result->CommArray[0]); /* &Result->CommArray[Result->pipeNum]);*/
//	free_info(Result);
	return Result;
}

void print_info (parseInfo *info) {
	printf("print_info: printing info about parseInfo struct\n");
	printf("Command: %s\n", info->CommArray[0].command);


	for (int i = 0; i < info->CommArray[0].VarNum; i++) {
		printf("Arg[%d]: %s\n", i, info->CommArray[0].VarList[i]);
	}
}
 
void free_info (parseInfo *info) {
	printf("free_info: freeing memory associated to parseInfo struct\n");
	free(info);
}

