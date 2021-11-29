//gcc file.c -lpthread -o programa
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
volatile int j = 0;
sem_t sem;
void *th_func1(void *arg) {
    int i;
    for(i = 0; i < 10000; i++) {
        sem_wait(&sem); //Entrada en la sección crítica
        j = j + 1;
        sem_post(&sem); //Salida de la sección crítica
    }
    pthread_exit(NULL);
}
void *primero(void *num){
    if((*(int*)num)==1){
        
        printf("primero");
        sem_post(&sem);
    }
    if((*(int*)num)==2){
        sem_wait(&sem);
        printf("segundo");
        sem_post(&sem);
    }
    
}
void *segundo(void *arg){
    sem_wait(&sem);
    printf("segundo");
    sem_post(&sem);
}
int main() {
    pthread_t th1, th2;
    sem_init(&sem, 0, 0);
    int a = 1;
    int b = 2;
    pthread_create(&th1, NULL, primero, (void*) &a);
    pthread_create(&th2, NULL, primero,(void*) &b);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    sem_destroy(&sem);
    // printf("j = %d\n", j); // j vale 20000
    return 0;
}