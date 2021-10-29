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
void singleComand(tcommand * command);
void multipleCommands(tline * line);
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
		if(line->ncommands==1){
			singleComand(line->commands);
		}
		else if(line->ncommands>1){
			multipleCommands(line);
			
		}
		// waitpid(pid,NULL,1);
		//wait(NULL);
		printf("msh %s> ",getcwd(buf, 1024)); fflush(stdout);
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

void singleComand(tcommand * command){
	pid_t pid = fork();
	int status;
	if(pid==0){
		if(command->filename!=NULL){
			execvp(command->filename, command->argv);
		}
		exit(1);
	}
	else{
		if(strcmp((command->argv)[0],"cd")==0){
			cd(command);
		}
		//waitpid(pid,&status,1);
		wait(NULL);
		
	}
	
}

void multipleCommands(tline * line){
	pid_t pid;
	int tuberia[2];
	int status;
	if(pipe(tuberia)){
		printf("error de tuberias");
	}
	for (int i = 0; i < line->ncommands; i++){
			printf("orden %d (%s):\n", i, line->commands[i].filename);
			for (int j = 0; j < line->commands[i].argc; j++)
			{
				printf("  argumento %d: %s\n", j, line->commands[i].argv[j]);
			}
			pid = fork();
			// char * aaa = (line->commands[i].argv)[0];
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
				else{
					exit(1);
				}		
			}
			else{
				wait(&status);
			}

	}
	for(int k=0;k<line->ncommands;k++){
		wait(&status);
	}
	// wait(&status);
	
}