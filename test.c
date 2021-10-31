#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#define _GNU_SOURCE
#define READ 0
#define WRITE 1
# define ERROR 2
#include "parser.h"
void cd(tcommand *command);
void singleComand(tline * line);
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
			singleComand(line);
		}
		else if(line->ncommands>1){
			multipleCommands(line);
		}
		fflush(stdout);
		usleep(2);
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

void singleComand(tline * line){
	pid_t pid = fork();
	int status;
	if(pid==0){
		if(line->redirect_input!=NULL){
			int file = open(line->redirect_input,O_RDONLY);
			if(file==-1){
				fprintf(stderr,"%s: Este fichero no existe o no se puede abrir.\n",line->redirect_input);
				exit(1);
			}else{
				dup2(file, READ);
			}
		}
		if(line->redirect_output!=NULL){
			int file = open(line->redirect_output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if(file==-1){
				fprintf(stderr,"%s: Fallo creando el fichero o no se puede abrir.\n",line->redirect_output);
				exit(1);
			}else{
				dup2(file, WRITE);
			}
		}
		if(line->redirect_error!=NULL){
			int file = open(line->redirect_error,O_WRONLY | O_CREAT | O_TRUNC);
			if(file==-1){
				fprintf(stderr,"%s: Fallo creando el fichero o no se puede abrir.\n",line->redirect_error);
				exit(1);
			}else{
				dup2(file, ERROR);
			}
		}	
		if(line->commands->filename!=NULL){
			execvp(line->commands->filename, line->commands->argv);
		}
		exit(1);
	}
	else{
		if(strcmp((line->commands->argv)[0],"cd")==0){
			cd(line->commands);
		}
		//waitpid(pid,&status,1);
		wait(NULL);
		
		
	}
	
}

void multipleCommands(tline * line){
	pid_t pid;
	int tuberia[2];
	
	int status;
	int wpid;
	if(pipe(tuberia)){
		printf("error de tuberias");
	}
	for (int i = 0; i < line->ncommands; i++){
			/* printf("orden %d (%s):\n", i, line->commands[i].filename);
			for (int j = 0; j < line->commands[i].argc; j++)
			{
				printf("  argumento %d: %s\n", j, line->commands[i].argv[j]);
			} */
			pid = fork();
			// char * aaa = (line->commands[i].argv)[0];
			//if(pid!=0) waitpid(0, &status, 1);
			if(pid==0){
				if(line->commands[i].filename!=NULL){
					if(i==0){
						
						dup2(tuberia[WRITE],WRITE);
						close(tuberia[READ]);
						close(tuberia[WRITE]);
						if(line->redirect_input!=NULL){
							int file = open(line->redirect_input,O_RDONLY);
							if(file==-1){
								fprintf(stderr,"%s: Este fichero no existe o no se puede abrir.\n",line->redirect_input);
								exit(1);
							}else{
								dup2(file, READ);
							}
						}	
						fflush(stdout);
						exit(execvp(line->commands[i].filename, (&(line->commands)[i])->argv));
					}else{
						dup2(tuberia[READ],READ);
						close(tuberia[READ]);
						close(tuberia[WRITE]);
						fflush(stdout);
						if(line->redirect_output!=NULL){
							int file = open(line->redirect_output,O_WRONLY | O_CREAT | O_TRUNC);
							if(file==-1){
								fprintf(stderr,"%s: Fallo creando el fichero o no se puede abrir.\n",line->redirect_output);
								exit(1);
							}else{
								dup2(file, WRITE);
							}
						}
						if(line->redirect_error!=NULL){
							int file = open(line->redirect_error,O_WRONLY | O_CREAT | O_TRUNC);
							if(file==-1){
								fprintf(stderr,"%s: Fallo creando el fichero o no se puede abrir.\n",line->redirect_error);
								exit(1);
							}else{
								dup2(file, ERROR);
							}
						}
						exit(execvp(line->commands[i].filename, (&(line->commands)[i])->argv));
					}
					
					//execlp(line->commands[i].filename,NULL);
				}
				else{
					exit(1);
				}		
			}
			else{
				//wait(&status);
				if(i==line->ncommands-1){
					waitpid(pid, NULL, 1);
					//usleep(2);
					//while(wait(NULL) > 0);
					//read(fin[0],NULL,sizeof(int));
				}
				
			}

	}
	//while(wait(NULL) > 0);
	
	/* for(int k=0;k<line->ncommands;k++){
		wait(&status);
	} */
	// wait(&status);
	
	
}

