#include "structs.h"
#include "IPC.h"
#include "scheduler.h"
#include "threads.h"
#include "buddyAllocator.h"
#include "stdioASM.h"



int createThread(void * entryPoint, void * args, Process * p)
{
  Thread * t = (Thread*) allocPage(NUMBER_OF_PAGES_TO_ALLOC(sizeof(Thread)));
  void * page1 = allocPage(2);
  void * page2 = allocPage(2);

  StackFrame * userStack=(StackFrame *)((char*)page1 + 2 * PAGE_SIZE)  ;
	StackFrame * kernelStack= (StackFrame *)((char*)page2 + 2 * PAGE_SIZE) ;
  StackFrame * stack= fillStackFrame(entryPoint, userStack, args);

  int index = findNextSpot(p->thread, 3);

  if(index == -1)
  {
    return -1;
  }

  p->numberOfThreads++;
  p->thread[index] = t;

  t->userStack = stack;
  t->entryPoint = entryPoint;
  t->kernelStack = kernelStack;

  return 0;


}

void terminateThread()
{
  Process * p = getCurrentProcess();
  if(p->activeThread == 0)
  {
    removeProcess(p->pid);
  }
  else
  {

    freeThreadPages(p->thread[p->activeThread]);
    p->thread[p->activeThread] = NULL;
    p->numberOfThreads--;
  }

}

int findNextSpot(Thread * t[], int dim)
{
    int i;
    for(i = 0; i < dim; i++)
    {
      if(t[i] == NULL)
      {
        t[i]=(Thread *) 1;
        return i; //Needs syncronization
      }
    }
    return -1;
}

void freeThreadPages(Thread * t)
{
  deallocPage((char*)t->userStack);
  deallocPage((char*)t->kernelStack);
  deallocPage((char*)t);
}

void thread_create(void * entryPoint, void * args)
{
    Process * p = getCurrentProcess();
    createThread(entryPoint, args, p);
}
