#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <setjmp.h>

void print_prompt();
int MAX_PATH = 1024;

//	Readline template source: https://en.wikipedia.org/wiki/GNU_Readline
int main()
{
    // Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_complete);

    while (1) {
        // Display prompt and read input
		print_prompt();
        char* input = readline("");

        // Check for EOF.
        if (!input)
            break;

        // Add input to readline history.
        add_history(input);

        // Do stuff...


        // Free buffer that was allocated by readline
        free(input);
    }
    return 0;
}


void print_prompt() {
	char *cwd, *host = NULL;
	size_t allocSize = sizeof(char) * MAX_PATH;
	cwd = (char *)malloc(allocSize);

//	Source: https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux
//	Gets user id
	printf("%s@", getlogin());

//	Source: https://stackoverflow.com/questions/504810/how-do-i-find-the-current-machines-full-hostname-in-c-hostname-and-domain-info
//	Gets hostname of machine
	host = (char *)malloc(allocSize);
	gethostname(host, allocSize);
	printf("%s:", host);

//	Source: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
//	Gets current path of user
	getcwd(cwd, allocSize);
	if (getcwd(cwd, allocSize) != NULL) {
		printf("%s$ ", cwd);
	}
	else {
		perror("getcwd() error");
	}

	free(cwd);
	free(host);
}

