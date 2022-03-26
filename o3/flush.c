#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_PATH 200
#define MAX_ARGS 20
#define MAX_ARG_LENGTH 40
#define MAX_COMMAND_LENGTH 40

int main(int argc, char const *argv[])
{
    char dir[MAX_PATH];
    char args[MAX_ARGS][MAX_ARG_LENGTH];
    char raw_input[MAX_COMMAND_LENGTH+MAX_ARGS*MAX_ARG_LENGTH];
    char command[MAX_COMMAND_LENGTH];
    char arg[MAX_ARG_LENGTH];
    while(1){
        for(int i = 0; i<MAX_ARGS; i++){
            memset(args[i], 0, MAX_ARG_LENGTH);
        }
        memset(raw_input, 0, MAX_COMMAND_LENGTH+MAX_ARGS*MAX_ARG_LENGTH);
        memset(command, 0, MAX_COMMAND_LENGTH);
        getcwd(dir, 200);
        printf("%s:",dir);
        fgets(raw_input, MAX_COMMAND_LENGTH+MAX_ARGS+MAX_ARG_LENGTH, stdin);
        strcpy(command, strtok(raw_input, " "));
        strcpy(arg, strtok(raw_input, " "));
        int i = 0;
        while(arg!=NULL){
            strcpy(args[i], arg);
            strcpy(arg, strtok(raw_input, " "));
            i++;
        }
        printf("%s\n", command);
        //for(int i = 0; i<MAX_ARGS; i++)
            //printf("%s\n", *(args[i]));
        

    }
    return 0;
}
