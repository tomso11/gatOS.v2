//IPC.h
#ifndef IPC_H

#define IPC_H

#define PIPE_LENGTH 10
#define MAX_PIPES 50
#define MAX_PROCESS_PIPES 3
#define MAX_LISTENERS_IPC 2

#define NO_SPACE_LEFT -1
#define PIPE_NOT_FOUND -2

#include "structs.h"
#include "lib.h"
#include "types.h"
#include "mutex.h"

Pipe * createPipeStruct(Process * pro,char * name);

void assignPipe(Pipe * pipe, Process * p);

char * next( char * aux, Pipe * pipe);

Process *  getProcessById(int pid);

void unlockProcesses(char * name);

void changeProcessState(int pid, processState state);

int getCurrentPid();

int getIndexForPipeStructure(Process * pro);

void freeSpace(char * name , Process * pro);

int getPipeNameIndex(char * name);

//API-FOR-USERS

int write(Pipe * p , char * message);

int read ( Pipe * p , char * result, int bytes);

void closePipe(Pipe * pipe);

Pipe *  createPipe( char * name);

Pipe *  openPipe(char * name);

#endif
