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
void executeexternal(tcommand * command,int morecoming);
int main(void)
{
	char buf[1024];
	tline *line;
	int i, j;
	pid_t pid;
	int tuberia[2];
	if(pipe(tuberia)){
		printf("error de tuberias");
	}
	
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
					if(i==0){
						
						dup2(tuberia[1],1);
						close(tuberia[0]);
						close(tuberia[1]);	
						execvp(line->commands[i].filename, (&(line->commands)[i])->argv);
						exit(1);
					}else{
						dup2(tuberia[0],0);
						close(tuberia[0]);
						close(tuberia[1]);
						execvp(line->commands[i].filename, (&(line->commands)[i])->argv);
						exit(1);
					}
					
					//execlp(line->commands[i].filename,NULL);
				}
				
				
				
			}else{
				if(strcmp((line->commands[i].argv)[0],"cd")==0){
					cd(&(line->commands)[i]);
				}
				waitpid(pid,NULL,1);
				
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
	int result=0;
	if(command->argc==1){
		result=chdir(getenv("HOME"));
	}if(command->argc>2){
		printf("Demasiados argumentos, puede que te refieras a: `cd %s`\n",command->argv[1]);
	}else{
		result=chdir(command->argv[1]);
	}
	if(result){
		printf("Error al moverse al directorio %s\n",command->argv[1]);
	}
}
