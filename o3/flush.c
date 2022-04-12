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
#define MAX_BACKGROUND_PROCESSES 20

struct Background_Process {
    int pid;
    char command_line[MAX_PATH];
};

struct Background_Process background_processes[MAX_BACKGROUND_PROCESSES];
int active_background_processes = 0;

void add_background_process (int pid, char *command_line)
{
    struct Background_Process new_background_process;
    new_background_process.pid = pid;
    strcpy(new_background_process.command_line, command_line);background_processes[active_background_processes] = new_background_process;
    active_background_processes++;
}

void remove_background_process (int pid)
{
    //Removes processes matchin pid from the array, but does not terminate anything.
    int found = 0;
    for (size_t i = 0; i < active_background_processes; i++)
    {
        if (background_processes[i].pid == pid)
        {
            found++;
        }
        else if (found)
        {
            background_processes[i-found] = background_processes[i];
        }
    }
    active_background_processes-= found;
    
    
}

void check_for_zombies()
{
    int pid;
    int status;

    //Resolve each zombie
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        remove_background_process(pid);
        printf("Process %d ended. Exit status: %d\n", pid, status);
    }
}

void display_jobs()
{
    puts("Active background processes:");
    for (size_t i = 0; i < active_background_processes; i++)
    {
        printf("[%d] %s\n", background_processes[i].pid, background_processes[i].command_line);
    }
}

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
    int start = -1;
    int end = -1;
    for(int i = 0; i<strlen(string); i++){
        if(start==-1 && isspace(string[i])==0){
            start = i;
        }
        if(!isspace(string[i])){
            end = i;
       }
    }
    memcpy(string, &string[start], end-start+1);
    string[end-start+1] = 0;
}

void execute_cmd()
{
    char raw_input[MAX_COMMAND_LENGTH + MAX_ARGS * MAX_ARG_LENGTH];
    
    fgets(raw_input, MAX_COMMAND_LENGTH + MAX_ARGS + MAX_ARG_LENGTH, stdin);
    raw_input[strcspn(raw_input, "\n")] = 0;

    char *token_pipe = strtok(raw_input, "|");
    while (token_pipe != NULL)
    {   
        int is_background_process = token_pipe[strlen(token_pipe) - 1] == '&';

        if (is_background_process)
        {
            token_pipe[strlen(token_pipe) - 1] = '\0';
        }

        int j = 0;
        int input_index, output_index, end_index;
        input_index = output_index = end_index = -1;
        char input_file_name[MAX_ARG_LENGTH];
        char output_file_name[MAX_ARG_LENGTH];
        memset(input_file_name, 0, MAX_ARG_LENGTH);
        memset(output_file_name, 0, MAX_ARG_LENGTH);
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
        // printf("command: %s\n",command_with_args);
        // printf("out: %s\n",output_file_name);
        // printf("in: %s\n",input_file_name);
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
                strip(output_file_name);
                int out = open(output_file_name, O_WRONLY | O_CREAT, S_IRWXU);
                dup2(out, STDOUT_FILENO);
            }
            if (input_index > 0)
            {
                strip(input_file_name);
                int in = open(input_file_name, O_RDONLY);
                dup2(in, STDIN_FILENO);
            }
            status = execvp(args_cpy[0], args_cpy);
            _exit(status);
        }
        else if (is_background_process)
        {
            add_background_process(pid, token_pipe);
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
        check_for_zombies();
        // Print current working directory
        getcwd(dir, MAX_PATH);
        printf("%s: ", dir);
        execute_cmd();
    }

    return 0;
}
