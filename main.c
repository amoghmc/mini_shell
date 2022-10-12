#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>

int main()
{
    // Configure readline to auto-complete paths when the tab key is hit.
    rl_bind_key('\t', rl_complete);
    char cwd[100];
    printf("hello");
    while (1) {
        // Display prompt and read input
        getcwd(cwd, sizeof(cwd));
        char* input = readline("");


        // Check for EOF.
        if (!input)

            break;

        // Add input to readline history.
        add_history(input);

        // Do stuff...
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s ", cwd);
        } else {
            perror("getcwd() error");
            //return 1;
        }
        

        // Free buffer that was allocated by readline
        free(input);
    }
    return 0;
}

