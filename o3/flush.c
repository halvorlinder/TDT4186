#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_PATH 200
#define MAX_ARGS 20
#define MAX_ARG_LENGTH 40
#define MAX_COMMAND_LENGTH 10

void change_dir(char *cwd, char *input)
{
    if (!input)
    {
        chdir(getenv("HOME"));
    }
    else
    {
        chdir(input);
    }
}

int main(int argc, char const *argv[])
{
    char dir[MAX_PATH];
    char args[MAX_ARGS][MAX_ARG_LENGTH];
    char raw_input[MAX_COMMAND_LENGTH + MAX_ARGS * MAX_ARG_LENGTH];

    while (1)
    {
        // Print current working directory
        getcwd(dir, MAX_PATH);
        printf("%s: ", dir);

        // Get input from user, and extract the first "word", also called a token
        fgets(raw_input, MAX_COMMAND_LENGTH + MAX_ARGS + MAX_ARG_LENGTH, stdin);
        raw_input[strcspn(raw_input, "\n")] = 0;

        char *token = strtok(raw_input, " ");

        int i = 0;
        while (token != NULL)
        {
            strcpy(args[i], token);
            token = strtok(NULL, " ");
            i++;
        }
        

        char* args_cpy[i];
        for(int j = 0; j<i; j++)
        {
            args_cpy[j] = malloc(MAX_ARG_LENGTH);
            strcpy(args_cpy[j], args[j]);
        }
        args_cpy[i] = NULL;
        if (!strcmp(args_cpy[0], "cd"))
        {
            change_dir(dir, args_cpy[1]);
        }
        else
        {
            int status;
            int pid = fork();
            if (pid == 0)
            {
                status = execv(args_cpy[0], args_cpy);
                _exit(status);
            }
            else
            {
                waitpid(pid, &status, 0);
                printf("Exit status: %d\n", status);
            }
        }
    }

    return 0;
}
