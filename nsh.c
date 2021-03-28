#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define _BUFSIZE 1024
#define _MAX_ARGS 20
#define _SHELL_NAME "nsh > "

typedef struct command{
    FILE* input;
    FILE* output;
    int output_type;
    int background;
    int pipe;
    char* arg_name;
    //struct command* next;
    char* argv[_MAX_ARGS];
}Command;

char* format_cmd(char* cmd)
{
    cmd=strtok(cmd,"\n");
    if(cmd==NULL){
        return NULL;
    }

    char* formated_cmd=(char*)malloc(_BUFSIZE*sizeof(char));
    int i=0,j=0,len=strlen(cmd);

    while(j!=len){
        if(cmd[j]!='>'&&cmd[j]!='<'&&cmd[j]!='|'&&cmd[j]!='&'){
            formated_cmd[i++]=cmd[j++];
        }
        else if(cmd[j]=='>'||cmd[j]=='>'||cmd[j]=='|'||cmd[j]=='&'){
            formated_cmd[i++]=' ';
            formated_cmd[i++]=cmd[j++];
            if(cmd[j-1]=='>'&&cmd[j]=='>'){
                formated_cmd[i++]=cmd[j++];
            }
            formated_cmd[i++]=' ';
        }
    }
    return formated_cmd;
}

Command* parse_command(char* buf)
{
    if(buf==NULL){
        return NULL;
    }
    char* token;
    token=strtok(buf," ");
    if(token==NULL){
        return NULL;
    }

    Command* command=(Command*)malloc(sizeof(Command));
    command->arg_name=(char*)malloc(sizeof(char)*strlen(token));
    strcpy(command->arg_name,token);
    command->input=stdin;
    command->output=stdout;
    command->output_type=0;
    command->background=0;
    command->pipe=0;
    int i=0;
    while(token!=NULL){
        command->argv[i]=(char*)malloc(sizeof(char)*strlen(token));
        strcpy(command->argv[i],token);
        i++;
        token=strtok(NULL," ");
    }
    command->argv[i]=NULL;
    free(buf);
    return command;
}

void execute_command(Command* command)
{
    int pid=fork();
    if(!strcmp(command->arg_name,"exit")){
        free(command);
        exit(0);
    }
    else{
        if(pid<0){
            perror("fork error!\n");
        }
        else if(pid==0){
            execvp(command->arg_name,command->argv);
        }
        else{
            int wait_ret=wait(NULL);
        }
    }
    free(command);
    return;
}

int main(int argc, char** argv)
{
    char* buf=(char*)malloc(_BUFSIZE*sizeof(char));
    fputs("Welcome to NaiveShell!\n",stdout);

    while(1){
        fputs(_SHELL_NAME, stdout);
        memset(buf,0,_BUFSIZE);

        fgets(buf, _BUFSIZE, stdin);
        Command* command=parse_command(format_cmd(buf));
        if(command==NULL){
            continue;
        }
        execute_command(command);
    }

    return 0;
}