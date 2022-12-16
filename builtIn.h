// header guards to prevent double inclusion
#ifndef BUILTIN_H
#define BUILTIN_H

#include <readline/history.h>
#include <termios.h>
#include "parse.h"

// Source for job control:
// https://www.gnu.org/software/libc/manual/html_node/Implementing-a-Shell.html


/* A process is a single process.  */
typedef struct process {
	struct process *next;       /* next process in pipeline */
	char **argv;                /* for exec */
	pid_t pid;                  /* process ID */
	char completed;             /* true if process has completed */
	char stopped;               /* true if process has stopped */
	int status;                 /* reported status value */
} process;

/* A job is a pipeline of processes.  */
typedef struct job {
	struct job *next;           /* next active job */
	char *command;              /* command line, used for messages */
	process *first_process;     /* list of processes in this job */
	pid_t pgid;                 /* process group ID */
	char notified;              /* true if user told about stopped job */
	struct termios tmodes;      /* saved terminal modes */
	int stdin, stdout, stderr;  /* standard i/o channels */
} job;

job *find_job(pid_t pgid);

int job_is_stopped(job *j);

int job_is_completed(job *j);

int isBuiltInCommand(char *command);

void executeBuiltInCommand(commandType *command, int type, HISTORY_STATE *historyState);

enum
BUILTIN_COMMANDS {
	EXIT, CD, HISTORY, JOBS, KILL, LEN, NO_SUCH_BUILTIN
};

#endif