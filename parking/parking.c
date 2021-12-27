#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>
#define PLAZAS 10
#define COCHES 8
#define CAMIONES 3
#define PLANTAS 2

int parking[PLANTAS][PLAZAS];
pthread_mutex_t aparcando;
int ocupados;
pthread_cond_t full, entradacamion;
sem_t sem;
void imprimirParking();

void *aparcacoche(void *args);
void *aparcacamion(void *args);

int main (int argc, char* argv[]){
	int matricoche[COCHES];
	int matricamion[CAMIONES];
	//int arraycamiones[CAMIONES];
	ocupados=0;
	pthread_t coche[COCHES];
	pthread_t camion[CAMIONES];
	pthread_mutex_init(&aparcando, NULL);
	pthread_cond_init(&full, NULL);
	pthread_cond_init(&entradacamion, NULL);
	sem_init(&sem, 0, 0);
	for(int i = 0; i<PLANTAS; i++){
		for(int j = 0; j<PLAZAS; j ++){
			parking[i][j]=0;
		}
	}
	for(int i=0;i<COCHES;i++){
		matricoche[i]=i+1;
		printf("coche %d creado", matricoche[i]);
		if(pthread_create(&coche[i], NULL, aparcacoche, (void*) &matricoche[i])){
			perror("Hilo no creao");
		}
	}
	for(int i=0;i<CAMIONES;i++){
		matricamion[i]=i+1+100;
		if(pthread_create(&camion[i], NULL, aparcacamion, (void*) &matricamion[i])){
			perror("Hilo no creao");
		}
	}
	//pthread_cond_signal(&full);

	while(1);
}

void imprimirParking(){
	printf("Parking: ");
		for (int i=0; i<PLANTAS; i++) {
			for(int j=0; j<PLAZAS; j++){
				printf("[%d] ", parking[i][j]);
			}
			printf("\n");
		}
	printf("\n");
}

void *aparcacoche(void *args){
	int matricula=*((int*) args);
	int libres;
	int pos;
	
    sleep((rand()%11)+1);
	while(1){
		//sleep((rand()%11)+1);
		pthread_mutex_lock(&aparcando);
		//printf("esperando al parkin %d\n", matricula);
		while(ocupados == PLAZAS*PLANTAS){
			pthread_cond_wait(&full, &aparcando);
		}
		
		pos = 0;
		while(parking[pos]!=0){
			pos++;
		}
		parking[pos]=matricula;
		ocupados++;
		printf("ENTRA -> 🚗Coche🚗 %d aparca en: %d. Plazas libres: %d\n",matricula,pos, PLAZAS-ocupados);
		imprimirParking();
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
		pthread_mutex_lock(&aparcando);
		ocupados--;
		parking[pos] = 0;
		printf("SALE <- 🚗Coche🚗 %d saliendo. Plazas libres: %d\n",matricula, PLAZAS-ocupados);
		pos = 0;
		while((parking[pos]!=0 || parking[pos+1]!=0) && pos<PLAZAS-1){
			pos++;
		}
		if(parking[pos+1]==0&&pos+1<=PLAZAS-1){
				pthread_cond_signal(&entradacamion);
		}
		imprimirParking();
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
	}
}

void *aparcacamion(void *args){
	int matricula=*((int*) args);
	int libres;
	int pos;
	int lib;
	int encontrado;
    sleep((rand()%11)+1);
	while(1){
		//sleep((rand()%11)+1);
		
		pthread_mutex_lock(&aparcando);
		encontrado=0;
		do{
			pthread_cond_wait(&entradacamion, &aparcando);
			pos = 0;
			while((parking[pos]!=0 || parking[pos+1]!=0) && pos<PLAZAS-1){  //[0] [1] [7] [3] [102] [102] [4] [103] [103] [0] ... 0
				pos++;														
			}
			if(parking[pos+1]==0&&pos+1<=PLAZAS-1){
				encontrado++;
			}
		}while(ocupados >= PLAZAS-1&&encontrado==0);
		parking[pos]=matricula;
		parking[pos+1]=matricula;
		ocupados++;
		ocupados++;
		printf("ENTRA -> 🚚Camion🚚 %d aparca en: %d y %d. Plazas libres: %d\n",matricula, pos, pos+1, PLAZAS-ocupados);
		imprimirParking();
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
		pthread_mutex_lock(&aparcando);
		ocupados--;
		ocupados--;
		parking[pos] = 0;
		parking[pos+1]=0;
		printf("SALE <- 🚚Camion🚚 %d saliendo. PLazas libres: %d\n",matricula, PLAZAS-ocupados);
		imprimirParking();
		pthread_cond_signal(&entradacamion);
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&aparcando);
		
		sleep((rand()%11)+1);
	}
}