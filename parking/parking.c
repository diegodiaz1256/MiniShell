#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#define PLAZAS 10
#define COCHES 8
#define CAMIONES 3

int parking[PLAZAS];
pthread_mutex_t aparcando;
int ocupados;
pthread_cond_t full;

void *aparcacoche(void *args){
	printf("acabo de entrar al parkin");
	//int matricula=*((int*) args);
	// int libres;
	// int pos;
	

	// while(1){
	// 	sleep((rand()%10)+1);
		// pthread_mutex_lock(&aparcando);
		// printf("acabo de entrar al parkin %d", matricula);
		// if(ocupados == PLAZAS){
		// 	pthread_cond_wait(&full, &aparcando);
		// }
		// printf("soy el coche %d",matricula);
		// sleep((rand()%11)+1);
		
		// //pthread_cond_signal(&full);
		// pthread_mutex_unlock(&aparcando);

		
	//}
}

int main (int argc, char* argv[]){
	int matricula[COCHES];
	//int arraycamiones[CAMIONES];
	ocupados=0;
	pthread_t coche[COCHES];
	pthread_mutex_init(&aparcando, NULL);
	pthread_cond_init(&full, NULL);
	for(int i = 0; i<PLAZAS; i++){
		parking[i]=-1;
	}
	for(int i=0;i<COCHES;i++){
		matricula[i]=i+1;
	}
	for(int i =0; i<COCHES;i++){
		if(pthread_create(&coche[i], NULL, aparcacoche, (void*) &matricula[i])){
			perror("Hilo no creao");
		}else{
			printf("coche %d\n", matricula[i]);
		}
	}
	while(1);
}

