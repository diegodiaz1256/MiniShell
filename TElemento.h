#ifndef TELEMENTO_H
#define TELEMENTO_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct process
{
	int job_id;		
	char job_name[1024];
	int status;
	pid_t pid;
}TElemento;

void asignar(TElemento* copia, TElemento original);
int igual(TElemento* uno, TElemento* dos);

void crear(char* n, pid_t pid,TElemento* e);

#endif
