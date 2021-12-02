#include <stdio.h>
#include <string.h>
#include "TElemento.h"
void asignar(TElemento* copia, TElemento original){
    strcpy((*copia).job_name,original.job_name);
    (*copia).job_id=original.job_id;
    copia->pid=original.pid;
    copia->status=original.status;
}
void crear(char* n, pid_t pid,TElemento* e){
    e->pid=pid;
    strcpy(n,e->job_name);
}
int igual(TElemento* uno, TElemento* dos){
    return uno->pid==dos->pid;
}

