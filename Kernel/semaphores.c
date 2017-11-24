//semaphores.c

#include "semaphores.h"

static Semaphore semaphores[MAX_SEMAPHORES];

static int semaphoreMutex; 

void initializeSemaphores()
{
	int i;

	for( i = 0 ; i < MAX_SEMAPHORES ; i++){

		cleanSemaphore(i);

	}

	semaphoreMutex = getMutex("semaphoreMutex");
}


void cleanSemaphore(int i)
{
	Semaphore * sem = &semaphores[i];

	sem->used = FALSE;
	sem->semaphore = 0;
	sem->cardinalBlocked = 0;

	clearArray(sem->queue , MAX_BLOCKED_QUEUE_SIZE , NO_BLOCKED_QUEUE_VALUE);
}

int isValidSemaphore(int index)
{

	if(index >= MAX_SEMAPHORES){
		return FALSE;
	}

	return semaphores[index].used;
}

int isSemaphoreQueueFull(int index)
{
	return semaphores[index].cardinalBlocked >= MAX_BLOCKED_QUEUE_SIZE ;
}

void addToSemBlocked( int i , int pid)
{
	Semaphore * sem = &semaphores[i];
	int index = getFreeSpotInBlockedQueue(sem->queue , MAX_BLOCKED_QUEUE_SIZE);

	if( index == NO_SPACE_LEFT_M){

		//PROCESS SHOULD BE KILLED
		ncPrint("MUERTE3");
		return;
	}
	sem->queue[index] = pid;
	sem->cardinalBlocked++;

}

int getSemIndex(char * mutexName)
{
	int i; 

	for(i = 0 ; i < MAX_SEMAPHORES ; i++){
		if( (strcmp(semaphores[i].name , mutexName)==0) && semaphores[i].used == TRUE){
			return i;
		}

	}

	return SEMAPHORE_NOT_FOUND;
}

int semOpen(char * name)
{
	int index;

	index = getSemIndex(name);

	if(index == MUTEX_NOT_FOUND){
		return ERROR;
	}

	return index;


}

void semClose(int index)
{
	int pid; 

	if(!isValidSemaphore(index)) return;

	pid = getCurrentPid();

	if(pid == semaphores[index].pidCreator ){
		semaphores[index].used = FALSE;
	}

}

int semCreate(char * name , int start)
{

	int index;

	lockMutex(semaphoreMutex);

	index = getSemIndex(name);

	if(index != MUTEX_NOT_FOUND){
		return ERROR;
	}
	
	index = createSemaphore(name,start);

	freeMutex(semaphoreMutex);

	return index;

}

int createSemaphore(char * semName , int start)
{
	int pid = getCurrentPid();

	int index = getFreePositionSemaphores();

	if(index == NO_SPACE_LEFT_M) return ERROR;

	semaphores[index].used = TRUE;

	semaphores[index].name = semName;

	semaphores[index].mutex = getMutex(semName);

	semaphores[index].semaphore = start;

	semaphores[index].pidCreator = pid ;

	return index;
}

int getFreePositionSemaphores()
{
	int i;

	for( i = 0 ; i < MAX_SEMAPHORES ; i++ ){
		if(semaphores[i].used == FALSE){
			return i;
		}
	}

	return NO_SPACE_LEFT_M;

}

void addToBlockedQueueSemaphore( int  sem , int pid)
{
	Semaphore * s = &semaphores[sem];
	int index = getFreeSpotInBlockedQueue(s->queue , MAX_BLOCKED_QUEUE_SIZE);

	if( index == NO_SPACE_LEFT_M){

		//PROCESS SHOULD BE KILLED
		ncPrint("MUERTE2");
		return;
	}
	s->queue[index] = pid;
	s->cardinalBlocked++;

}

void wait(int semaphore)
{

	int pid;

	Semaphore * sem ;

	if(!isValidSemaphore(semaphore)) return;

	sem = &semaphores[semaphore];

	lockMutex(sem->mutex);

	sem->semaphore--;

	if(sem->semaphore >= 0){

		freeMutex(sem->mutex);
		
	}else{

		pid = getCurrentPid();

		addToSemBlocked(semaphore , pid );

		changeProcessState(pid, BLOCKED);

		freeMutex(sem->mutex);

		_yield();


	}
	
}

void signal(int semaphore)
{

	Semaphore * sem ;

	if(!isValidSemaphore(semaphore)){
	 	return;
	}

	sem = &semaphores[semaphore] ;	

	if(sem->cardinalBlocked > 0 ){
			unblockProcessSemaphore(semaphore);
			sem->cardinalBlocked--;
	}

	sem->semaphore++;


}

void unblockProcessSemaphore( int sem)
{
	int i;

	Semaphore * s = &semaphores[sem]; 

	for( i = 0 ; i < MAX_BLOCKED_QUEUE_SIZE ; i++){
		if(s->queue[i] != NO_BLOCKED_QUEUE_VALUE){
			changeProcessState( s->queue[i] , READY );
			s->queue[i] = NO_BLOCKED_QUEUE_VALUE;
			return;
		}
	}

}

