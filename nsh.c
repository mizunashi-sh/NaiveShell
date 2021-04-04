#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define _BUFSIZE 1024
#define _MAX_ARGS 20
#define _SHELL_NAME "nsh > "

#define COMMAND_NOT_FOUND 0
#define FORK_ERROR 1
#define FILE_DIR_NOT_FOUND 2

typedef struct command
{
    FILE *input;
    FILE *output;
    int output_type;
    int background;
    int pipe;
    //struct command* next;
    char *argv[_MAX_ARGS];
} Command;

void error_handling(int type)
{
    switch (type)
    {
        char error_message[90];
    case COMMAND_NOT_FOUND:
        strcpy(error_message, "nsh: command not found\n");
        write(STDERR_FILENO, error_message, strlen(error_message));
        break;
    case FORK_ERROR:
        strcpy(error_message, "nsh: fork error\n");
        write(STDERR_FILENO, error_message, strlen(error_message));
        break;
    case FILE_DIR_NOT_FOUND:
        strcpy(error_message, "nsh: no such file or directory\n");
        write(STDERR_FILENO, error_message, strlen(error_message));
        break;
    default:
        strcpy(error_message, "nsh: an error has occurred\n");
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
}

char *format_cmd(char *cmd)
{
    cmd = strtok(cmd, "\n");
    if (cmd == NULL)
    {
        return NULL;
    }

    char *formated_cmd = (char *)malloc(_BUFSIZE * sizeof(char));
    memset(formated_cmd, 0, _BUFSIZE);
    int i = 0, j = 0, len = strlen(cmd);

    while (j != len)
    {
        if (cmd[j] != '>' && cmd[j] != '<' && cmd[j] != '|' && cmd[j] != '&')
        {
            formated_cmd[i++] = cmd[j++];
        }
        else if (cmd[j] == '>' || cmd[j] == '>' || cmd[j] == '|' || cmd[j] == '&')
        {
            formated_cmd[i++] = ' ';
            formated_cmd[i++] = cmd[j++];
            if (cmd[j - 1] == '>' && cmd[j] == '>')
            {
                formated_cmd[i++] = cmd[j++];
            }
            formated_cmd[i++] = ' ';
        }
    }
    return formated_cmd;
}

Command *parse_command(char *buf)
{
    if (buf == NULL)
    {
        return NULL;
    }
    char *token;
    token = strtok(buf, " ");
    if (token == NULL)
    {
        return NULL;
    }

    Command *command = (Command *)malloc(sizeof(Command));
    command->input = stdin;
    command->output = stdout;
    command->output_type = 0;
    command->background = 0;
    command->pipe = 0;
    for(int j=0;j<_MAX_ARGS;j++){
        command->argv[j]=NULL;
    }
    int i = 0;
    while (token != NULL)
    {
        command->argv[i] = (char *)malloc(sizeof(char) * strlen(token));
        strcpy(command->argv[i], token);
        i++;
        token = strtok(NULL, " ");
    }
    free(buf);
    return command;
}

void execute_command(Command *command)
{
    if (!strcmp(command->argv[0], "exit"))
    {
        fputs("exit\n", stdout);
        exit(0);
    }
    else if (!strcmp(command->argv[0], "cd"))
    {
        char *dir=NULL;
        if(command->argv[1]==NULL||!strcmp(command->argv[1],"~"))
            dir=getenv("HOME");
        else
            dir=command->argv[1];
        if(chdir(dir)<0)
            error_handling(FILE_DIR_NOT_FOUND);
    }
    else if(!strcmp(command->argv[0],"pwd")){
        char cwd[_BUFSIZE];
        getcwd(cwd,_BUFSIZE);
        fputs(cwd,command->output);
        fputs("\n",command->output);
    }
    else if(!strcmp(command->argv[0],"wait")){
        wait(NULL);
    }
    else
    {
        int pid = fork();
        if (pid < 0)
        {
            error_handling(FORK_ERROR);
        }
        else if (pid == 0)
        {
            if (execvp(command->argv[0], command->argv) < 0)
                error_handling(COMMAND_NOT_FOUND);
            _exit(0);
        }
        else
        {
            int wait_ret = wait(NULL);
        }
    }
}

int main(int argc, char **argv)
{
    char *buf = (char *)malloc(_BUFSIZE * sizeof(char));

    while (1)
    {
        fputs(_SHELL_NAME, stdout);

        memset(buf, 0, _BUFSIZE);
        fgets(buf, _BUFSIZE, stdin);
        Command *command = parse_command(format_cmd(buf));
        if (command == NULL)
        {
            continue;
        }
        execute_command(command);
        for (int i = 0; i < _MAX_ARGS; i++)
        {
            if (command->argv[i] != NULL)
                free(command->argv[i]);
        }
        free(command);
    }

    return 0;
}
