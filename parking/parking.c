#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>
int PLAZAS;
int COCHES;
int CAMIONES;
int PLANTAS;

int ** parking;
pthread_mutex_t aparcando;
int ocupados;
pthread_cond_t full, entradacamion;
void imprimirParking();

void *aparcacoche(void *args);
void *aparcacamion(void *args);

int main (int argc, char* argv[]){

	if(argc==1){
		PLAZAS = 4;
		COCHES = 8;
		CAMIONES = 2;
		PLANTAS = 1;	
	}else if(argc==3){
		PLAZAS= atoi(argv[1]);
		PLANTAS = atoi(argv[2]);
		COCHES = 2*PLANTAS*PLAZAS;
		CAMIONES = 0;
	}else if(argc==4){
		PLAZAS= atoi(argv[1]);
		COCHES = atoi(argv[3]);
		PLANTAS = atoi(argv[2]);
		CAMIONES = 0;	
	}else if(argc==5){
		PLAZAS= atoi(argv[1]);
		COCHES = atoi(argv[3]);
		PLANTAS = atoi(argv[2]);
		CAMIONES = atoi(argv[4]);	
	}else{
		printf("Entrada no valida");
	}

	parking = (int**)malloc(sizeof(int*)*(PLANTAS));
	for(int i =0; i<PLANTAS;i++){
		parking[i]=(int*) malloc(sizeof(int)*PLAZAS);
	}	

	printf("+++++++++++++++\nPLAZAS=%d\nPLANTAS=%d\nCOCHES=%d\nCAMIONES=%d\n+++++++++++++++\n", PLAZAS, PLANTAS, COCHES, CAMIONES);
	sleep(2);

	int matricoche[COCHES];
	int matricamion[CAMIONES];
	ocupados=0;
	pthread_t coche[COCHES];
	pthread_t camion[CAMIONES];
	pthread_mutex_init(&aparcando, NULL);
	pthread_cond_init(&full, NULL);
	pthread_cond_init(&entradacamion, NULL);
	for(int i = 0; i<PLANTAS; i++){
		for(int j = 0; j<PLAZAS; j ++){
			parking[i][j]=0;
		}
	}
	for(int i=0;i<COCHES;i++){
		matricoche[i]=i+1;
		if(pthread_create(&coche[i], NULL, aparcacoche, (void*) &matricoche[i])){
			perror("Error: Hilo no creado");
		}
	}
	for(int i=0;i<CAMIONES;i++){
		matricamion[i]=i+1+100;
		if(pthread_create(&camion[i], NULL, aparcacamion, (void*) &matricamion[i])){
			perror("Error: Hilo no creado");
		}
	}
	while(1);
}

void imprimirParking(){
	printf("Parking: ");
		for (int i=0; i<PLANTAS; i++) {
			for(int j=0; j<PLAZAS; j++){
				printf("[%d] ", parking[i][j]);
			}
			printf("\n         ");
		}
	printf("\n");
}

void *aparcacoche(void *args){
	int matricula=*((int*) args);
	int libres;
	int pos;
	int planta;
    sleep((rand()%11)+1);
	while(1){
		pthread_mutex_lock(&aparcando);
		while(ocupados == PLAZAS*PLANTAS){
			pthread_cond_wait(&full, &aparcando);
		}
		
		pos = 0;
		planta = 0;
		while(parking[planta][pos]!=0){
			pos++;
			if(pos==PLAZAS && planta < PLANTAS-1){
				pos=0;
				planta++;
			}
		}
		parking[planta][pos]=matricula;
		ocupados++;
		// 🚓🏍️🏎️🦼🚗🚕🚙🏎️🚓🚜🦽🦼
		char coches[10][10] =  {"🚓","🏍️","🦼","🚗","🚕","🚙","🏎️","🚓","🚜","🦽"};
		char * coche = coches[rand()%10];
		printf("ENTRA -> %s  Coche %s  %d aparca en: plaza %d de planta %d. Plazas libres: %d\n",coche,coche,matricula,pos,planta, (PLAZAS*PLANTAS)-ocupados);
		imprimirParking();
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
		pthread_mutex_lock(&aparcando);
		ocupados--;
		parking[planta][pos] = 0;
		printf("SALE <- %s  Coche %s  %d saliendo. Plazas libres: %d\n",coche, coche, matricula, (PLAZAS*PLANTAS)-ocupados);
		for(int i=0; i<PLANTAS;i++){
			pos=0;
			while((parking[i][pos]!=0 || parking[i][pos+1]!=0) && pos<PLAZAS-1){
				pos++;
			}
			if(parking[i][pos+1]==0 && parking[i][pos] == 0 &&pos<PLAZAS-1){
				pthread_cond_signal(&entradacamion);
				break;
			}
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
	int planta;
	int encontrado;
    sleep((rand()%11)+1);
	while(1){
		pthread_mutex_lock(&aparcando);
		encontrado=0;
		do{
			for(int i=0; i<PLANTAS; i++){
				pos = 0;
				while((parking[i][pos]!=0 || parking[i][pos+1]!=0) && pos<PLAZAS-1){ 
					pos++;														
				}
				if(parking[i][pos+1]==0 && parking[i][pos] == 0 &&pos<PLAZAS-1){
					encontrado++;
					planta = i;
					break;
				}
			}
			if(encontrado==0){
				pthread_cond_wait(&entradacamion, &aparcando);
			}
		}while(ocupados >= (PLAZAS*PLANTAS)-1||encontrado==0);
		parking[planta][pos]=matricula;
		parking[planta][pos+1]=matricula;
		ocupados++;
		ocupados++;
		//🚚🚛🚌🚎🚟🚋🚝🚅✈️🛺
		char camiones[10][10] = {"🚚","🚛","🚌","🚎","🚟","🚋","🚝","🚅","✈️","🛺"};
		char * camion = camiones[rand()%10];
		printf("ENTRA -> %s  Camion %s  %d aparca en: %d y %d de la planta %d. Plazas libres: %d\n",camion,camion,matricula, pos, pos+1, planta, (PLAZAS*PLANTAS)-ocupados);
		imprimirParking();
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
		pthread_mutex_lock(&aparcando);
		ocupados--;
		ocupados--;
		parking[planta][pos] = 0;
		parking[planta][pos+1]=0;
		printf("SALE <- %s  Camion %s  %d saliendo. PLazas libres: %d\n",camion,camion,matricula, (PLAZAS*PLANTAS)-ocupados);
		imprimirParking();
		pthread_cond_signal(&entradacamion);
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&aparcando);
		sleep((rand()%11)+1);
	}
}