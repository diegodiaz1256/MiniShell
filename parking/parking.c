#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#define PLAZAS 6
#define COCHES 9
#define CAMIONES 3

int parking[PLAZAS];
pthread_mutex_t aparcando;
int ocupados;
pthread_cond_t full;
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
	for(int i = 0; i<PLAZAS; i++){
		parking[i]=0;
	}
	for(int i=0;i<COCHES;i++){
		matricoche[i]=i+1;
		if(pthread_create(&coche[i], NULL, aparcacoche, (void*) &matricoche[i])){
			perror("Hilo no creao");
		}
	}
	for(int i=0;i<COCHES;i++){
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
		for (int i=0; i<PLAZAS; i++) {
			printf("[%d] ", parking[i]); //estado actual del parking
		}
	printf("\n");
}

void *aparcacoche(void *args){
	int matricula=((int) args);
	int libres;
	int pos;
	
    sleep((rand()%11)+1);
	while(1){
		//sleep((rand()%11)+1);
		pthread_mutex_lock(&aparcando);
		//printf("esperando al parkin %d\n", matricula);
		if(ocupados == PLAZAS){
			pthread_cond_wait(&full, &aparcando);
		}
		
		pos = 0;
		while(parking[pos]!=0){
			pos++;
		}
		parking[pos]=matricula;
		ocupados++;
		printf("ENTRA -> el coche %d libres:%d\n",matricula, PLAZAS-ocupados);
		imprimirParking();
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
		pthread_mutex_lock(&aparcando);
		ocupados--;
		parking[pos] = -1;
		printf("SALE <- coche %d libres: %d\n",matricula, PLAZAS-ocupados);
		imprimirParking();
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
	}
}

void *aparcacamion(void *args){
	int matricula=((int) args);
	int libres;
	int pos;
	
    sleep((rand()%11)+1);
	while(1){
		//sleep((rand()%11)+1);
		pthread_mutex_lock(&aparcando);
		//printf("esperando al parkin %d\n", matricula);
		if(ocupados >= PLAZAS-1){
			pthread_cond_wait(&full, &aparcando);
		}
		
		pos = 0;
		while(parking[pos]!=0 && parking[pos+1]!=0){
			pos++;
		}
		parking[pos]=matricula;
		ocupados++;
		printf("ENTRA -> el coche %d libres:%d\n",matricula, PLAZAS-ocupados);
		imprimirParking();
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
		pthread_mutex_lock(&aparcando);
		ocupados--;
		parking[pos] = -1;
		printf("SALE <- coche %d libres: %d\n",matricula, PLAZAS-ocupados);
		imprimirParking();
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
}