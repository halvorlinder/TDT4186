#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>


#define MAX_PATH 200
#define MAX_ARGS 20
#define MAX_ARG_LENGTH 40
#define MAX_COMMAND_LENGTH 10
#define MAX_PIPELINE_LENGTH 10

void change_dir(char *input)
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

void strip(char* string){
    puts("hello");
    int start, end = -1;
    for(int i = 0; i<strlen(string); i++){
        if(start==-1 && !isspace(string[i])){
            start = i;
        }
        if(!isspace(string[i])){
            end = i;
       }
    }
    string[end+1] = 0;
    memcpy(string, &string[start], end-start+1);
}

void execute_cmd()
{
    char raw_input[MAX_COMMAND_LENGTH + MAX_ARGS * MAX_ARG_LENGTH];

    fgets(raw_input, MAX_COMMAND_LENGTH + MAX_ARGS + MAX_ARG_LENGTH, stdin);
    raw_input[strcspn(raw_input, "\n")] = 0;

    char *token_pipe = strtok(raw_input, "|");
    while (token_pipe != NULL)
    {
        int j = 0;
        int input_index, output_index, end_index;
        input_index = output_index = end_index = -1;
        char input_file_name[MAX_ARG_LENGTH];
        char output_file_name[MAX_ARG_LENGTH];
        // if cd or jobs do something
        //  if (!strcmp(args_cpy[0], "cd"))
        //  {
        //      change_dir(args_cpy[1]);
        //  }

        //Find the indices of the redirection operators
        // puts(token_pipe);
        while (j < MAX_ARG_LENGTH)
        {
            // printf("%c\n", token_pipe[j]);
            if (token_pipe[j] == '>')
            {
                output_index = j;
            }
            else if (token_pipe[j] == '<')
            {
                input_index = j;
            }
            else if (token_pipe[j] == 0)
            {
                end_index = j;
                break;
            }
            j++;
            }

        // printf("%d\n", input_index);
        // printf("%d\n", output_index);
        // printf("%d\n", end_index);
        
        //Get the file names of the input/output files 
        if (output_index > 0 && input_index > 0)
        {
            if (output_index > input_index)
            {
                memcpy(input_file_name, &token_pipe[input_index + 1], output_index-input_index-1);
                memcpy(output_file_name, &token_pipe[output_index + 1], end_index-output_index-1);
            }
            else
            {
                memcpy(output_file_name, &token_pipe[output_index + 1], input_index-output_index-1);
                memcpy(input_file_name, &token_pipe[input_index + 1], end_index-input_index-1);
            }
        }
        else if (output_index > 0)
        {
            memcpy(output_file_name, &token_pipe[output_index + 1], end_index-output_index-1);
        }
        else if (input_index > 0)
        {
            memcpy(input_file_name, &token_pipe[input_index + 1], end_index-input_index-1);
        }
        
        char *command_with_args = strtok(token_pipe, "<>");
        puts(command_with_args);
        puts(output_file_name);
        puts(input_file_name);
        //Parse the command 
        char args[MAX_ARGS][MAX_ARG_LENGTH];
        char *token = strtok(command_with_args, " ");
        int i = 0;
        while (token != NULL)
        {
            strcpy(args[i], token);
            token = strtok(NULL, " ");
            i++;
        }

        char *args_cpy[i];
        for (int j = 0; j < i; j++)
        {
            args_cpy[j] = malloc(MAX_ARG_LENGTH);
            strcpy(args_cpy[j], args[j]);
        }

        args_cpy[i] = NULL;

        int status;
        int pid = fork();
        if (pid == 0)
        {
            if (output_index > 0)
            {
                // strip(output_file_name);
                int out = open(output_file_name, O_WRONLY);
                dup2(out, STDOUT_FILENO);
            }
            if (input_index > 0)
            {
                // strip(input_file_name);
                int in = open(input_file_name, O_RDONLY);
                dup2(in, STDIN_FILENO);
            }
            status = execvp(args_cpy[0], args_cpy);
            _exit(status);
        }
        else
        {
            waitpid(pid, &status, 0);
            printf("Exit status: %d\n", status);
        }

        for (int j = 0; j < i; j++)
        {
            free(args_cpy[j]);
        }
        i++;
        token_pipe = strtok(NULL, "|");
    }
}

int main(int argc, char const *argv[])
{

    char dir[MAX_PATH];
    while (1)
    {
        // Print current working directory
        getcwd(dir, MAX_PATH);
        printf("%s: ", dir);
        execute_cmd();
    }

    return 0;
}
