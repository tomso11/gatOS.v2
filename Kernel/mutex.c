//mutex.c

#include "mutex.h"
#include "queue.h"
#include "stdio.h"

#define MAX_MUTEXES 128

#define MUTEX_NAME_LEN 64

#define OPEN 1
#define CLOSED 0

#define LOCKED 0
#define UNLOCKED 1

#define NOT_OPEN_ERROR -1
#define MAX_MUTEX_OPEN_ERROR -2

typedef struct {
  char name[MUTEX_NAME_LEN];
  int64_t locked;
  int64_t state;
  queueADT process_queue;
} mutex_type;

static mutex_type open_mutexes[MAX_MUTEXES];
/* special lock para mutex array*/
static int64_t array_mutex = UNLOCKED;
static int64_t superlock = UNLOCKED;

/* previene que se creen dos procesos al mismo tiempo*/
/*
void initialize_process_mutex() {
	table_mutex_key = mutex_open("__PROCESS_TABLE_MUTEX__");
}
*/
extern int _unlocked(int64_t * locknum);

void set_superlock() {
	superlock = LOCKED;
}

void unset_superlock() {
	superlock = UNLOCKED;
}

/* +++ Manejo de special lock mutex array  +++ */
static void lock_array() {
  while (!_unlocked(&array_mutex))
    //yield_process();
    _yield();
}

static void unlock_array() {
  array_mutex = UNLOCKED;
}

/* ++++++++++++++++++++++++++++++++++++++++++ */

static mutex_type create_new_mutex(char * name) {
  mutex_type m;
  strcpy(m.name, name);
  m.process_queue = create_queue();
  m.state = OPEN;
  m.locked = UNLOCKED;
  return m;
}

static void queue_process(mutex_type *m, Process * p) {
  enqueue(m->process_queue, p);
}

static Process * dequeue_process(mutex_type *m) {
  return dequeue(m->process_queue);
}

static int is_open(int key) {
  return key >= 0 && key < MAX_MUTEXES && open_mutexes[key].state == OPEN;
}


/* ------------------MUTEX USER API---------------------------- */

int mutex_open(char * name) {
  int k;

  lock_array();

  for (k = 0; k < MAX_MUTEXES && is_open(k); k++) {
    if (strcmp(name, open_mutexes[k].name) == 0) {
      unlock_array();
      return k;
    }
  }

  if (k == MAX_MUTEXES) {
    unlock_array();
    return MAX_MUTEX_OPEN_ERROR;
  }

  open_mutexes[k] = create_new_mutex(name);

  unlock_array();

  return k;
}

int mutex_close(int key) {
  lock_array();
  Process * p;

  if (is_open(key)) {

    mutex_type * m = &open_mutexes[key];

    while (!is_empty(m->process_queue)) // Hay procesos lockeados
      //unblock_process(dequeue_process(m));
    	p=dequeue_process(m);
    	changeProcessState(p->pid, READY);

    m->state = CLOSED;
    destroy_queue(m->process_queue);

    unlock_array();
    return 1;
  }

  unlock_array();
  return NOT_OPEN_ERROR;
}

int mutex_lock(int key) {
  if (is_open(key)) {

    set_superlock();

    mutex_type *m = &open_mutexes[key];
    Process * p = getCurrentProcess();

    if (!_unlocked(&m->locked)) {

    /* Encolar y bloquear debe ser atomico.
    ** Si se encola y no bloquea, puede que se desencole y desbloquee y luego bloquearse.
    ** Si se bloquea y no encola, como esta bloqueado no se encolara, entonces no se desencolara y nunca desbloqueara.
    ** El superlock asegura que el proceso actual volvera a correr dada una interrupcion de timer tick. */

      queue_process(m, p);

      //block_process(p);
      changeProcessState(p->pid, BLOCKED);

      unset_superlock();
      //yield_process();
      _yield();
    }
    else
      unset_superlock();

    return 1;
  }
  return NOT_OPEN_ERROR;
}

int mutex_unlock(int key) {
  if (is_open(key)) {

    set_superlock();

    mutex_type *m = &open_mutexes[key];

    Process * p = dequeue_process(m);

    /*no se si se maneja asi esta comp*/
    while (p->state == 3)
      p = dequeue_process(m);

    m->locked = UNLOCKED;

    if (p != NULL) {
      //unblock_process(p);
    	changeProcessState(p->pid, READY);
      m->locked = LOCKED;
    }

    unset_superlock();

    return 1;
  }
  return NOT_OPEN_ERROR;
}

/* ------------------------------------------------------------------------------------- */

/* OLD IMPLEMENTATION */

/* --------------------------------------------------------------------------------*/


//__________________________________________________________________________
//__________________________________________________________________________
//FUNCTIONS RELATED TO ADMINISTRATION OF MUTEXES

static Mutex mutexes[MAX_MUTEXES];
static int adminMutex ;
void initializeMutexes()
{
	int i;

	for( i = 0 ; i < MAX_MUTEXES ; i++){

		cleanMutex(i);
	}

	adminMutex = 0;
	mutexes[adminMutex].used=TRUE;

}

int getIndex(char * mutexName)
{
	int i; 

	for(i = 0 ; i < MAX_MUTEXES ; i++){
		if( (strcmp(mutexes[i].name , mutexName)==0) && mutexes[i].used == TRUE){
			return i;
		}

	}

	return MUTEX_NOT_FOUND;
}

int getMutex(char * mutexName)
{
	int index;

	lockMutex(adminMutex);

	index = getIndex(mutexName);

	if( index == MUTEX_NOT_FOUND){

		index = createMutex(mutexName);

	}

	freeMutex(adminMutex);

	return index;
}

int createMutex(char * mutexName)
{
	int pid= getCurrentPid();

	int index = getFreeSpot();

	if(index == NO_SPACE_LEFT_M) return ERROR;

	mutexes[index].used = TRUE;

	mutexes[index].name = mutexName;

	mutexes[index].pidCreator = pid;

	return index;
}

int getFreeSpot()
{
	int i;

	for( i = 0 ; i < MAX_MUTEXES ; i++ ){
		if(mutexes[i].used == FALSE){
			return i;
		}
	}

	return NO_SPACE_LEFT_M;

}

void cleanMutex(int index)
{
	if(index >= MAX_MUTEXES) return;

	Mutex * m = &mutexes[index];

	m->used = FALSE;
	m->name = NULL;
	m->cardinalBlocked = 0;
	m->mutex = 0 ; 

	clearArray(m->queue , MAX_BLOCKED_QUEUE_SIZE , NO_BLOCKED_QUEUE_VALUE);

}

void clearArray(int * array , int length , int value)
{
	int i ;

	for( i = 0 ; i < length ; i++){
		array[i] = value;
	}
}

//______________________________________________________________________________________________
//______________________________________________________________________________________________
//FUNCTIONS TO WORK WITH MUTEXES

int isValidMutex(int mutex)
{

	if(mutex >= MAX_MUTEXES){
		return FALSE;
	}

	return mutexes[mutex].used;
}

int isQueueFull(int mutex)
{
	return mutexes[mutex].cardinalBlocked >= MAX_BLOCKED_QUEUE_SIZE ;
}

int getFreeSpotInBlockedQueue(int * blocked , int length)
{
	int i;

	for( i = 0 ; i < length ; i++ ){
		if(blocked[i] == NO_BLOCKED_QUEUE_VALUE){
			return i;
		}
	}

	return NO_SPACE_LEFT_M;
}

void addToBlocked( int  mutex , int pid)
{
	Mutex * m = &mutexes[mutex];
	int index = getFreeSpotInBlockedQueue(m->queue , MAX_BLOCKED_QUEUE_SIZE);

	if( index == NO_SPACE_LEFT_M){

		//PROCESS SHOULD BE KILLED
		ncPrint("MUERTE5");
		return;
	}
	m->queue[index] = pid;
	m->cardinalBlocked++;

}

int getFreeSpotInUsingQueue(int * using , int length)
{
	int i;

	for( i = 0 ; i < length ; i++ ){
		if(using[i] == NO_PID_VALUE){
			return i;
		}
	}

	return NO_SPACE_LEFT_M;
}

void lockMutex(int mutex)
{
	int lock;
	int pid;

	if(!isValidMutex(mutex)) return;

	lock = _testAndSet(&(mutexes[mutex].mutex));

	if(lock != ACQUIRED){

		if(isQueueFull(mutex)){
			//ERROR DEBERIAMOS MATAR EL PROCESO
		}else{
			
			pid = getCurrentPid();
			addToBlocked( mutex , pid );

		}

		
		//MUTEX

		changeProcessState(pid, BLOCKED);

		_yield();


	}


}

void freeMutex(int mutex)
{
	if(!isValidMutex(mutex)){
	 	return;
	} 	

	if(mutexes[mutex].cardinalBlocked > 0 ){
			unblockProcess(mutex);
			mutexes[mutex].cardinalBlocked--;
	}else{
		mutexes[mutex].mutex=0;
	}


}

void unblockProcess( int mutex )
{
	int i;

	Mutex * m = &mutexes[mutex]; 

	for( i = 0 ; i < MAX_BLOCKED_QUEUE_SIZE ; i++){
		if(m->queue[i] != NO_BLOCKED_QUEUE_VALUE){
			changeProcessState( m->queue[i] , READY );
			m->queue[i] = NO_BLOCKED_QUEUE_VALUE;
			return;
		}
	}

}


void closeMutex(int index)
{
	int pid; 

	if(!isValidMutex(index)) return;

	pid = getCurrentPid();

	if(pid == mutexes[index].pidCreator ){
		mutexes[index].used = FALSE;
	}

}