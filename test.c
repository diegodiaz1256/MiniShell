#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define _GNU_SOURCE
#include "parser.h"

int main(void)
{
	char buf[1024];
	tline *line;
	int i, j;
	pid_t pid;

	printf("msh> ");
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
		}
		if (line->ncommands > 0){
			pid = fork();
			if(pid==0){
				execvp(line->commands[0].filename, line->commands->argv);
				exit(0);
			}else{
				waitpid(pid,NULL,0);
			}
		}
		
		printf("msh> ");
	}
	return 0;
}
