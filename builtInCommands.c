#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "builtIn.h"

void change_dir(char *path);

job *first_job = NULL;

const char *builtInArray[LEN] = {
		"exit",
		"cd",
		"history",
		"jobs",
		"kill"
};

int isBuiltInCommand(char *command) {
	for (int i = EXIT; i < LEN; i++) {
		if (strncmp(command, builtInArray[i], strlen(builtInArray[i])) == 0) {
			return i;
		}
	}
	return NO_SUCH_BUILTIN;
}

void executeBuiltInCommand(commandType *command, int type, HISTORY_STATE *history_state) {
	switch (type) {
		case CD:
			change_dir(command->VarList[1]);
			break;
		case HISTORY:
//			Source: https://stackoverflow.com/questions/38792542/readline-h-history-usage-in-c
			for (int i = 0; i < history_state->length; i++) {
				printf(" %8s  %s", history_state->entries[i]->line, history_state->entries[i]->timestamp);
			}
			putchar('\n');
			break;
		case JOBS:
//			todo
			break;
		case KILL:
//			todo
			break;
		default:
			printf("\nError! No such builtin command!\n");
	}
	free(history_state);
}

void change_dir(char *path) {
	int status = chdir(path);
	if (status) {
		printf("\nError! No such path exists!\n");
	}
}

/* Find the active job with the indicated pgid.  */
job *find_job(pid_t pgid) {
	job *j;

	for (j = first_job; j; j = j->next)
		if (j->pgid == pgid)
			return j;
	return NULL;
}

/* Return true if all processes in the job have stopped or completed.  */
int job_is_stopped(job *j) {
	process *p;

	for (p = j->first_process; p; p = p->next)
		if (!p->completed && !p->stopped)
			return 0;
	return 1;
}

/* Return true if all processes in the job have completed.  */
int job_is_completed(job *j) {
	process *p;

	for (p = j->first_process; p; p = p->next)
		if (!p->completed)
			return 0;
	return 1;
}

