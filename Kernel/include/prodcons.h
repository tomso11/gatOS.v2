#ifndef PRODCONS_H
#define PRODCONS_H

#include "semaphores.h"
#include "mutex.h"

#define EMPTY 0
#define FULL 1

#define DEFAULTCANTPROD 1
#define DEFAULTCANTCONS 1 
#define BUFFERLENGTH 10

struct threadArgumentPC {
	mutex_t * mutex;
    sem_t * semaphores[2];
    int value;
};
typedef struct threadArgumentPC threadArgumentPC;

//int strlen(char * c);
//int isdigit(char c);
//int isNotInteger(char * c);
void insertItem(char c);
char removeItem();
void * producer(void* args);
void * consumer(void* args);
void createThreadsPC(int cantProd, int cantCons, mutex_t * mutex, sem_t * emptyCount, sem_t * fullCount);
int createSem(sem_t * sem, const char * semName, int value);
int createSemaphores(sem_t * emptyCount, sem_t * fullCount, const char * emptyCountName, const char * fullCountName);
void endSemaphores(sem_t emptyCount, sem_t fullCount);
int prodCons(int cantProd, int cantCons, mutex_t * mutex, sem_t * emptyCount, sem_t * fullCount, const char * mutexName, const char * emptyCountName, const char * fullCountName);
int validateParametersPC(int cant, char *toValidate[], int * cantProd, int * cantCons);

//para la prueba
int mainPC();

#endif