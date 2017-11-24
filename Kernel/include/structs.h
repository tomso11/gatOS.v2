//structs.h
#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_PAGES_PER_PROCESS 10
#define MAX_THREAD 10
#include <lib.h>


typedef struct t_block * s_block;

typedef struct t_block{
	size_t size;
 	s_block next;
 	s_block prev;
 	int free;
 	char dataStart[1];
}t_block;

typedef enum processState_t {RUNNING, READY, BLOCKED, DEAD, SLEEPING} processState;

typedef struct Pipe_s {
	char * name;
	int position;
	int end;
	int creator;
	int * pipePids;
	int * blocked;
	int cardinalBlocked;
	int connected;
	char * pipe;
	int readFlag;
	int mutex;
}Pipe;

typedef struct StackFrame_s{
	//Registers restore context
	uint64_t gs;
	uint64_t fs;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	//iretq hook
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags;
	uint64_t rsp;
	uint64_t ss;
	uint64_t base;
}StackFrame;

typedef struct Thread_s{
	StackFrame * userStack;
	StackFrame * kernelStack;
	void * entryPoint;
}Thread;

typedef struct Process_s{
	int pid;
	processState state;
	char * description;

	int activeThread;
	int numberOfThreads;

	Thread * thread[MAX_THREAD];

	int pipesOpened;
 	int * occupiedPosition;
	int * pipePids;
	int * blocked;
	Pipe *  pipesStruct;
	char *  pipes;
	s_block  heap;
	int pagesIndex;
	void * pages[MAX_PAGES_PER_PROCESS];
}Process;





#endif
