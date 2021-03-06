#include <stdio.h>
#include <stdlib.h>
#include "TElemento.h"



struct Nodo{
    TElemento info;
    struct Nodo* sig;
};

typedef struct Nodo Tnodo;

typedef Tnodo* TLinkedList;
void imprimir(TLinkedList l);
void sacar(TLinkedList* l, TElemento* e, int num);
void crearVacia (TLinkedList* a);
void mostrar (TLinkedList a);
void insertar (TElemento e, TLinkedList* a);
void insertarFinal (TElemento e, TLinkedList* a);
void eliminar (int id, TLinkedList* a, TElemento * salida);
void primero (TLinkedList a, TElemento* e);
void resto (TLinkedList* a);
void ultimo (TLinkedList a, TElemento* e);
void concatenar(TLinkedList* a1, TLinkedList a2);
int contiene (TElemento e, TLinkedList a);
int esVacia (TLinkedList a);
int longitud (TLinkedList a);

void copiar(TLinkedList original, TLinkedList* copia);
void destruir(TLinkedList* a);