#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#define _GNU_SOURCE
#include "parser.h"
void cd(tcommand *command);
int main(void)
{
	char buf[1024];
	tline *line;
	int i, j;
	pid_t pid;
	
	printf("msh %s> ",getcwd(buf, 1024));
	while (fgets(buf, 1024, stdin)){

		line = tokenize(buf);
		if (line == NULL){
			continue;
		}
		if (line->redirect_input != NULL){
			printf("redirección de entrada: %s\n", line->redirect_input);
		}
		if (line->redirect_output != NULL){
			printf("redirección de salida: %s\n", line->redirect_output);
		}
		if (line->redirect_error != NULL){
			printf("redirección de error: %s\n", line->redirect_error);
		}
		if (line->background){
			printf("comando a ejecutarse en background\n");
		}
		for (i = 0; i < line->ncommands; i++){
			printf("orden %d (%s):\n", i, line->commands[i].filename);
			for (j = 0; j < line->commands[i].argc; j++)
			{
				printf("  argumento %d: %s\n", j, line->commands[i].argv[j]);
			}
			pid = fork();
			char * aaa = (line->commands[i].argv)[0];
			if(pid==0){
				if(line->commands[i].filename!=NULL){
					execvp(line->commands[i].filename, line->commands->argv);
				}
				
				exit(0);
				
			}else{
				if(strcmp((line->commands[i].argv)[0],"cd")==0){
					cd(&(line->commands)[i]);
				}
				waitpid(pid,NULL,0);
			}
		}
		/* if (line->ncommands > 0){
			pid = fork();
			{if(pid==0){
				if(line->commands[0].filename!=NULL){
					char ** primero = line->commands->argv;
					execvp(line->commands[0].filename, line->commands->argv);
					sleep(2);
					exit(0);
				}
			}else{
				waitpid(pid,NULL,0);
			}}
		} */
		
		printf("msh %s> ",getcwd(buf, 1024));
	}
	return 0;
}

void cd(tcommand *command){
	if(command->argc==1){
		printf("\n\n--  %d  --\n\n",chdir(getenv("HOME")));
	}else{
		printf("\n\n--  %d  --\n\n",chdir(command->argv[1]));
	}
}
