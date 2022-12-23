#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "builtIn.h"

void change_dir(char *path);

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

void print_running_jobs(job *head) {
	job *current = head;
	printf("PID\t\tCommand\t\tStatus");
	while(current != NULL)
	{
		if (kill(current->pid, 0) == 0) {
			printf("%d\t\t%s\t\t", current->pid, current->command);
			if (waitpid(current->pid, 0, WNOHANG) == 0) {
				printf("Running");
			}
			else {
				printf("Stopped");
			}
			current = current->next;
		}
		else {
//			job* temp = current;
//			current = current->next;
//			free(temp);
		}
	}
}

void append_job(job *head, pid_t pid, char* cmd) {
	job *current = head;
	while (current->next != NULL) {
		current = current->next;
	}

	current->next = malloc(sizeof(job));
	current->next->pid = pid;
	current->next->next = NULL;
}

void free_jobs(job* head) {
	job* tmp;
	while (head != NULL)
	{
		tmp = head;
		head = head->next;
		free(tmp);
	}
}