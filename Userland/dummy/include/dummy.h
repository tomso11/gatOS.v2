#ifndef PRODCONS_H
#define PRODCONS_H

#define MAX_CONSUMER 10
#define MAX_PRODUCER 10

#define PROD_CONS 1
#define PIPE_TEST 2
#define MALLOC_TEST 3
#define PHILOSOPHERS 4


#define DEFAULTP 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define ERROR -1

typedef int sem_t;

typedef int mutex_t;

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
int philosphers();


void insertItem(char c);
char removeItem();
void * producer(void * ctx);
void * consumer(void * ctx);
void control();
void killAllProcesses();

#endif
