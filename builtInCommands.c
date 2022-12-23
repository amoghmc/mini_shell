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

void executeBuiltInCommand(commandType *command, int type, HISTORY_STATE *history_state, job* head) {
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
			print_running_jobs(head);
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
	printf("PID\t\tCommand\t\t\tStatus\n");
	int i = 0;
	while(current != NULL)
	{
//		check if process exist in the job list, 0 = true
		int kill_signal = kill(current->pid, 0);
//		check if process is running, 0 = true
		int wait_signal = waitpid(current->pid, 0, WNOHANG);
		if (((kill_signal == 0) && (wait_signal == 0)) && (current->pid != 0)) {
			printf("%d\t\t%s\t\t", current->pid, current->command);
			printf("Running");
			current = current->next;
			printf("\n");
		}
		else {
			current = current->next;
		}
		i++;
	}
}


// add job to the begining of the linked list of jobs
void add_job(job **head, pid_t pid, char cmd[])
{
	job *new_job = NULL;

	new_job = malloc(sizeof(job));
	new_job->pid = pid;
	new_job->next = *head;
	strcpy(new_job->command, cmd);

	*head = new_job;
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