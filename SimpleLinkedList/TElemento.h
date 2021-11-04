#ifndef TELEMENTO_H
#define TELEMENTO_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct TComando{
     pid_t pid;
     char* comando;
}TElemento;

char* salidacom (TElemento t);
pid_t salidapid(TElemento t);



void crear(char* n, pid_t pid,TElemento* e);

#endif
