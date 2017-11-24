//mutex.h

#ifndef MUTEX

#define MUTEX 0 

#include "stdio.h"
#include "lock.h"
#include "scheduler.h"
#include "naiveConsole.h"


#define MAX_BLOCKED_QUEUE_SIZE 20
#define MAX_MUTEX_PIDS 20

#define MAX_MUTEXES 100

#define FALSE 0
#define TRUE 1
#define MUTEX_NOT_FOUND -1
#define NO_SPACE_LEFT_M -2
#define NO_BLOCKED_QUEUE_VALUE -1
#define NO_PID_VALUE -1
#define ACQUIRED 1
#define ERROR -4

typedef struct {
	int used;
    char * name;
    int queue[MAX_BLOCKED_QUEUE_SIZE];
    int usingPids[MAX_MUTEX_PIDS];
    int mutex;
    int cardinalBlocked;
    int cardinalUsing;
    int pidCreator;
}Mutex;

typedef int mutex_t;




void initializeMutexes();
int getIndex(char * mutexName);
int getMutex(char * mutexName);
void addToUsing(int mutex , int pid);
int createMutex(char * mutexName);
int getFreeSpot();
void cleanMutex(int index);
void clearArray(int * array , int length , int value);
int isValidMutex(int mutex);
int isQueueFull(int mutex);
int getFreeSpotInBlockedQueue(int * blocked , int length);
void addToBlocked( int mutex , int pid);
int getFreeSpotInUsingQueue(int * using , int length);
void unblockProcess( int mutex );

//API-FOR-USERS

int getMutex(char * mutexName);

void lockMutex(int mutex);

void freeMutex (int mutex);

void closeMutex(int index);

#endif