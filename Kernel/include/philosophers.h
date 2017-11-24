#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

#include "mutex.h"
#include "semaphores.h"

#define DEFAULTP 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2

typedef struct{
	int cant;
    sem_t * sems;
    sem_t * mutex;
    int value;
}threadArgument;

int strlen(char * c);
int isdigit(char c);
int isNotInteger(char * c);
int validateParameters(int cant, char *toValidate[], int * input);
int startSemaphores(mutex_t * mutex, sem_t * semaphores[], int cant);
void testP(int number, int total, sem_t * sem);
void take_fork(threadArgument * arg);
void put_fork(threadArgument * arg);
void * philospher(void * args);
void createThreads(int total, sem_t * semaphores[], mutex_t * mutex);
void initializeArray(sem_t * array[], int dim);
int dPhilosphers(int number);

//para la prueba
int mainP();

#endif
