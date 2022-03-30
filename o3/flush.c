#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_PATH 200
#define MAX_ARGS 20
#define MAX_ARG_LENGTH 40
#define MAX_COMMAND_LENGTH 10

int main(int argc, char const *argv[])
{
    char dir[MAX_PATH];
    char args[MAX_ARGS][MAX_ARG_LENGTH];
    char raw_input[MAX_COMMAND_LENGTH + MAX_ARGS * MAX_ARG_LENGTH];
    char command[MAX_COMMAND_LENGTH];
    while (1)
    {
        for (int i = 0; i < MAX_ARGS; i++)
        {
            memset(args[i], 0, MAX_ARG_LENGTH);
        }
        memset(raw_input, 0, MAX_COMMAND_LENGTH + MAX_ARGS * MAX_ARG_LENGTH);
        memset(command, 0, MAX_COMMAND_LENGTH);
        getcwd(dir, 200);
        printf("%s:", dir);
        fgets(raw_input, MAX_COMMAND_LENGTH + MAX_ARGS + MAX_ARG_LENGTH, stdin);
        char *token = strtok(raw_input, " ");
        strcpy(command, token);
        token = strtok(NULL, " ");
        int i = 0;
        while (token != NULL)
        {
            strcpy(args[i], token);
            token = strtok(NULL, " ");
            i++;
        }

        int status;
        int pid = fork();
        if (pid == 0)
        {
            execl(command, command, args, NULL);
            _exit(0);
        }
        else
        {
            waitpid(pid, &status, 0);
            printf("Exit status: %d\n", status);
        }
    }
    return 0;
}
