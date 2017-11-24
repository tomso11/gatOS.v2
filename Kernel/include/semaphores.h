//semaphores.h

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#define SEMAPHORE 0

#include "mutex.h"

#define MAX_SEMAPHORE_PIDS 20

#define MAX_SEMAPHORES MAX_MUTEXES/2

#define SEMAPHORE_NOT_FOUND -1

typedef struct {
	int used;
    char * name;
    int queue[MAX_BLOCKED_QUEUE_SIZE];
    int semaphore;
    int cardinalBlocked;
   	int mutex;
   	int pidCreator;
}Semaphore;

typedef int sem_t;

void initializeSemaphores();

void cleanSemaphore(int i);

int isValidSemaphore(int index);

int isSemaphoreQueueFull(int index);

void addToSemBlocked( int index , int pid);

int getSemIndex(char * mutexName);

int createSemaphore(char * semName , int start);

int getFreePositionSemaphores();

void addToBlockedQueueSemaphore( int  sem , int pid);

void unblockProcessSemaphore( int sem);

//API_FOR USERS

void wait(int semaphore);

void signal(int semaphore);

int semOpen(char * name);

int semCreate(char * name , int start);

void semClose(int index); 

#endif