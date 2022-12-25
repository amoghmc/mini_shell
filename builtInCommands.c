#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include "builtIn.h"

void change_dir(char *path);

void kill_job(job *head, char *cmd);

bool parseLong(const char *str, long *val);

bool is_running(job *foo);

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

void executeBuiltInCommand(commandType *command, int type, HISTORY_STATE *history_state, job *head) {
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
			kill_job(head, command->VarList[1]);
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
	int check = false;
	while (current != NULL) {
		if (is_running(current)) {
			if (!check) {
				printf("PID\t\tCommand\t\t\tStatus\n");
				check = true;
			}
			printf("%d\t\t%s\t\tRunning", current->pid, current->command);
			current = current->next;
			printf("\n");
		} else {
			current = current->next;
		}
	}
	if (!check) {
		printf("jobs: There are no jobs running!\n");
	}
}


// add job to the beginning of the linked list of jobs
void add_job(job **head, pid_t pid, char cmd[]) {
	job *new_job = NULL;

	new_job = malloc(sizeof(job));
	new_job->pid = pid;
	new_job->next = *head;
	strcpy(new_job->command, cmd);

	*head = new_job;
}

void copy_running_jobs(job **head) {
	job *new = NULL;
	job *current = *head;
	while (current != NULL) {
		if (is_running(current)) {
			add_job(&new, current->pid, current->command);
		}
		current = current->next;
	}
	job *temp = *head;
	*head = new;
	free_jobs(temp);
}

void free_jobs(job *head) {
	job *tmp;
	while (head != NULL) {
		tmp = head;
		head = head->next;
		free(tmp);
	}
}


void kill_job(job *head, char *cmd) {
	job *current = head;
	long int pid = 0;
	if ((cmd != NULL) && (!parseLong(cmd, &pid))) {
		printf("Error: Invalid command!\n");
		return;
	}
	while (current != NULL) {
		if (current->pid == pid && is_running(current)) {
			kill(current->pid, SIGKILL);
			return;
		} else if (current->pid == pid) {
			printf("Error: No such job exists!\n");
			return;
		}
		current = current->next;
	}
	printf("Error: No such job exists!\n");
}

//	Source: https://stackoverflow.com/questions/14176123/correct-usage-of-strtol
bool parseLong(const char *str, long *val) {
	char *temp;
	bool rc = true;
	errno = 0;
	*val = strtol(str, &temp, 0);

	if (temp == str || *temp != '\0' ||
		((*val == LONG_MIN || *val == LONG_MAX) && errno == ERANGE))
		rc = false;

	return rc;
}

bool is_running(job *foo) {
	//	check if process exist in the job list, 0 = true
	int kill_signal = kill(foo->pid, 0);
	//	check if process is running, 0 = true
	int wait_signal = waitpid(foo->pid, 0, WNOHANG);
	if (kill_signal == 0 && wait_signal == 0) {
		return true;
	}
	return false;
}