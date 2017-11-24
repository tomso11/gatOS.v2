#include "process.h"
#include "threads.h"
#include "buddyAllocator.h"

static int pid = 0;

#define MAX_PAGES_PER_PROCESS 10

StackFrame * fillStackFrame(void * entryPoint, StackFrame * userStack, void * args){

	StackFrame * frame = userStack - 2;
	frame->gs =		0x001;
	frame->fs =		0x002;
	frame->r15 =	0x003;
	frame->r14 =	0x004;
	frame->r13 =	0x005;
	frame->r12 =	0x006;
	frame->r11 =	0x007;
	frame->r10 =	0x008;
	frame->r9 =		0x009;
	frame->r8 =		0x00A;
	frame->rsi =	(uint64_t )entryPoint;
	frame->rdi =	(uint64_t)entryPoint; //entryPoint;
	frame->rbp =	0x00D;
	frame->rdx =	(uint64_t)args;
	frame->rcx =	(uint64_t)args;
	frame->rbx =	0x010;
	frame->rax =	0x011;
	frame->rip =	(uint64_t) &callProcess;
	frame->cs =		0x008;
	frame->eflags = 0x202;
	frame->base =	(uint64_t) (userStack - 50 -1);
	frame->rsp =	(uint64_t)&(frame->base);
	frame->ss = 	0x000;

	return frame;

}

Process * getProcess(void * entryPoint , char * description, void * args){

	char * infoPage = allocPage(NUMBER_OF_PAGES_TO_ALLOC(sizeof(Process)));
	
	Process * p =(Process *) infoPage;

	infoPage = infoPage + sizeof(Process);

	p->activeThread = 0;
	p->numberOfThreads = 0;

	initializeThreadArray(p->thread, MAX_THREADS);
	createThread(entryPoint, args, p);

	p->pid = pid++;
	p->state = READY;
	p->description = description;
	p->heap = NULL;
	p->pagesIndex = 0;
	initializePipeFields(p , infoPage);

	initiatePipesForProcess(p->occupiedPosition);
	return p;

}

void initializePipeFields(Process * p , char  * infoPage)
{
	p->pipes = infoPage; //array de char  *
	p->pipesOpened = 0;
	p->pipesStruct = (Pipe*)((char*)p->pipes + MAX_PROCESS_PIPES*PIPE_LENGTH); //array de Pipesstruct
	p->pipePids  = (int*)((char *)p->pipesStruct + MAX_PROCESS_PIPES*sizeof(*p->pipesStruct) ); //array de pids
	p->blocked = (int*)((char*)p->pipePids + MAX_PROCESS_PIPES*MAX_LISTENERS_IPC*sizeof(int) ); //array de pids
	p->occupiedPosition = (int*)((char*)p->blocked + MAX_LISTENERS_IPC*MAX_PROCESS_PIPES*sizeof(int) ); //array de int
}

int freeProcessPages(int pid)
{
	int i;
	Process  * p = getProcessById(pid);
	deallocPage((char*)p);
	for(i=0; i<p->numberOfThreads; i++){
		deallocPage((char*)p->thread[i]->userStack);
		deallocPage((char*)p->thread[i]->kernelStack);
	}
	deallocPage((char*)p->heap->dataStart);
	for (i = 0; i < p->pagesIndex; ++i){
		deallocPage((char*)p->pages[i]);
	}
	return 0;
}

void initiatePagesForProcess(void * pages[] , int length)
{
	int i;
	for(i = 0; i < length; i++){
		pages[i] = NULL;
	}
}

void addPagesToProcess(void * pageBase,int pid){
	Process  * p = getProcessById(pid);
	if(p == NULL || p->pagesIndex >= MAX_PAGES_PER_PROCESS){
		return;
	}
	p->pages[p->pagesIndex] = pageBase;
	p->pagesIndex++;
}

void initiatePipesForProcess(int * occupiedPosition)
{
	int i;
	for(i = 0; i < MAX_PROCESS_PIPES; i++){
		occupiedPosition[i] = 0;
	}
}

void initializeThreadArray(Thread * t[], int dim){
	int i;
	for(i = 0; i < dim; i++){
		t[i] = NULL;
	}
}
