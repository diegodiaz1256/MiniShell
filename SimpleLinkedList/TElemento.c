#include <stdio.h>
#include <string.h>
#include "TElemento.h"
void asignar(TElemento* copia, TElemento original){
    strcpy((*copia).job_name,original.job_name);
    (*copia).job_id=original.job_id;
    copia->pgid=original.pgid;
    copia->pid=original.pid;
    copia->ppid=original.ppid;
    copia->status=original.status;
}
char * salidacom (TElemento t){
    return t.job_name;
}
pid_t salidapid (TElemento t){
    return t.pid;
}
void crear(char* n, pid_t pid,TElemento* e){
    e->pid=pid;
    strcpy(n,e->job_name);
}
int igual(TElemento* uno, TElemento* dos){
    return uno->pid==dos->pid;
}

