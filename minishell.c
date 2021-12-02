#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include "simplelinkedlist.h"
#include "parser.h"
#define _GNU_SOURCE
#define READ 0
#define WRITE 1
#define ERROR 2

//TLinkedList background;
static TLinkedList Background;
void cd(tcommand *command);
void singleComand(tline * line,bool isbg);
void multipleCommands(tline * line, bool isbg);
void addbg(tline* linea, char *name);
int main(void)
{
	crearVacia(&Background);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGINT,SIG_IGN);

	//TODO
	//crearVacia(&background);
	char buf[1024];
	tline *line;
	int i, j;
	pid_t pid;
	int tuberia[2];
	if(pipe(tuberia)){
		printf("error de tuberias");
	}
	printf("\nmsh %s> ",getcwd(buf, 1024));
	while (fgets(buf, 1024, stdin)){

		line = tokenize(buf);
		if (line == NULL){
			continue;
		}
		/* if (line->redirect_input != NULL){
			printf("redirección de entrada: %s\n", line->redirect_input);
		}
		if (line->redirect_output != NULL){
			printf("redirección de salida: %s\n", line->redirect_output);
		}
		if (line->redirect_error != NULL){
			printf("redirección de error: %s\n", line->redirect_error);
		} */
		printf("\n");
		if (line->background){
			printf("Entra background");
			//printf("comando a ejecutarse en background\n");
			addbg(line, buf);
		}
		else if(line->ncommands==1){
			singleComand(line,false);
		}
		else if(line->ncommands>1){
			multipleCommands(line, false);
			fflush(stdout);
			// usleep(2);
		}
			
		
		// waitpid(pid,NULL,1);
		//wait(NULL);
		printf("\nmsh %s> ",getcwd(buf, 1024)); fflush(stdout);
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

void singleComand(tline * line, bool isbg){
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
		}else if(strcmp((line->commands->argv)[0],"exit")==0){
			exit(0);
		}
		wait(&status);
		if(isbg){
			TElemento salida;
			eliminar(getpid(),&Background, &salida);
			//asignar(&salida, eliminar(getpid(),&Background));
			fprintf(stdout, "[%d]+ Ended\t\t %s", salida.job_id, salida.job_name);
			exit(0);
		}
	}
	
}



void multipleCommands(tline * line, bool isbg){
	// signal(SIGQUIT,SIG_DFL);
	// signal(SIGINT,SIG_DFL);
	
	pid_t pid;
	int tuberia[(line->ncommands)-1][2];
	// printf("numero de comandos: %d\n",line->ncommands);
	int status;
	int wpid;
	for(int i=0; i<(line->ncommands)-1; i++){
		if(pipe(tuberia[i])){
			printf("error de tuberias");
		}
	}

	if((pid=fork())==-1){
		fprintf(stderr,"Error");
		exit(1);
	}
	//primer comando
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
		close(tuberia[0][READ]);
		dup2(tuberia[0][WRITE], WRITE);
		exit(execvp(line->commands[0].filename, (&(line->commands)[0])->argv));
	}
	if(line->ncommands>=3){
		//comandos en medio
		
			for(int i=1; i<(line->ncommands)-1;i++){
				if((pid=fork())==-1){
					fprintf(stderr,"Error");
					exit(1);
				}
				if(pid==0){
					close(tuberia[i-1][WRITE]);
					close(tuberia[i][READ]);
					for(int j=0; j<((line->ncommands)-1); j++) {
						if(j!=i && j!=(i-1)) {
							close(tuberia[j][WRITE]);
							close(tuberia[j][READ]);
						}
					}
					dup2(tuberia[i-1][READ],READ);
					dup2(tuberia[i][WRITE],WRITE);
					if(line->commands[i].filename!=NULL){
						exit(execvp(line->commands[i].filename, (&(line->commands)[i])->argv));
					}else{
						fprintf(stderr,"Error Comando no encontrado");
						exit(1);
					}
				}
			}
		
	}
	if((pid=fork())==-1){
		fprintf(stderr,"Error");
		exit(1);
	}
	if(pid==0){
		//ultimo comando
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
		close(tuberia[line->ncommands-2][WRITE]);
		for (int j=0; j<((line->ncommands)-2); j++) {
			close(tuberia[j][WRITE]);
			close(tuberia[j][READ]);
		}
		dup2(tuberia[line->ncommands-2][READ], READ);
		exit(execvp(line->commands[line->ncommands-1].filename, (&(line->commands)[line->ncommands-1])->argv));
	}
	for (int j=0; j<((line->ncommands)-1); j++) {
		close(tuberia[j][WRITE]);
		close(tuberia[j][READ]);
	}
	for(int i=0; i<line->ncommands;i++){
		wait(NULL);
	}
	if(isbg){
		//TElemento salida=eliminar(getpid(),&Background);
		//fprintf(stdout, "[%d]+ Ended\t\t %s", salida.job_id, salida.job_name);
		exit(0);
	}
}

void addbg(tline * linea, char * name){
	pid_t papa;
	if((papa= fork())==-1){
		fprintf(stderr, "Error de ejecucion");
	}
	if(papa==0){
		printf("Entra hijo");
		if(linea->ncommands > 1){
			multipleCommands(linea, true);
		}
		else{
			singleComand(linea,true);
		}
	}else{
		printf("Entra padre");
		TElemento proc;
		printf("creadoelemento");
		strcpy(proc.job_name, name);
		proc.pid=papa;
		proc.status=0;
		printf("\n%d salida back \n", longitud(Background));
		proc.job_id=longitud(Background)+1;
			
		printf("todo asignado");
		insertar(proc, &Background);
		printf("insertao");
		printf("\n%d salida back \n", longitud(Background));
	}
}


