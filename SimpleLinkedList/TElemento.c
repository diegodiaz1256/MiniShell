#include <stdio.h>
#include <string.h>
#include "TElemento.h"

char * salidacom (TElemento t){
    return t.comando;
}
pid_t salidapid (TElemento t){
    return t.pid;
}
void crear(char* n, pid_t pid,TElemento* e){
    e->pid=pid;
    strcpy(n,e->comando);
}

