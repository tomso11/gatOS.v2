#ifndef THREADS_H
#define THREADS_H

#define MAX_THREADS 10
#include "structs.h"
#include "types.h"

typedef int pthread_t;

int createThread(void * entryPoint, void * args, Process * p);
void terminateThread();
int findNextSpot(Thread * t[], int dim);
void freeThreadPages(Thread * t);
void thread_create(void * entryPoint, void * args);

#endif
