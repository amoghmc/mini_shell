// header guards to prevent double inclusion
#ifndef BUILTIN_H
#define BUILTIN_H

#include <readline/history.h>
#include <termios.h>
#include "parse.h"

#define MAX_COM_SIZE 256

// Source for job control:
// https://www.gnu.org/software/libc/manual/html_node/Implementing-a-Shell.html

/* A job is a pipeline of processes.  */
typedef struct job {
	struct job *next;           /* next active job */
	char command[MAX_COM_SIZE]; /* command line, used for messages */
	pid_t pid;                  /* process ID */
//	char notified;              /* true if user told about stopped job */
} job;

int isBuiltInCommand(char *command);

void executeBuiltInCommand(commandType *command, int type, HISTORY_STATE *historyState, job *head);

void add_job(job **head, pid_t pid, char cmd[]);

void copy_running_jobs(job **head);

void free_jobs(job *head);

void print_running_jobs(job *head);

enum
BUILTIN_COMMANDS {
	EXIT, CD, HISTORY, JOBS, KILL, LEN, NO_SUCH_BUILTIN
};

#endif