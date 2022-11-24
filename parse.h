#define MAX_VAR_NUM 10
#define PIPE_MAX_NUM 10
#define FILE_MAX_SIZE 40

struct commandType {
	char *command;
	char *VarList[MAX_VAR_NUM];
	int VarNum;
    char inFile[FILE_MAX_SIZE];
    char outFile[FILE_MAX_SIZE];
    int boolInfile;
    int boolOutfile;
};

/* parsing information structure */
typedef struct {
	int boolBackground;                /* run the process in the background? */
	struct commandType CommArray[PIPE_MAX_NUM];
	int pipeNum;
} parseInfo;

/* the function prototypes */
parseInfo *parse(char *);

void free_info(parseInfo *);

void print_info(parseInfo *);

void parse_command(parseInfo *Result, char **res_space, int space_delims, int i);

char **split_string(char *cmdline, int *n_delims, char *delim);

void error_check(parseInfo *info, char **res_pipe, char **res_space, int type);

parseInfo *init_info(parseInfo *info);






