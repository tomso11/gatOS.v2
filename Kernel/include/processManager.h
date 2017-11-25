#ifndef _PROC_MANAGER_H_
#define _PROC_MANAGER_H_

#include <stdint.h>
#include "newProcess.h"


#define MAX_PROCESSES 128

uint64_t next_process(uint64_t current_rsp);

uint64_t exec_process(process * new_process);
void end_process();
void yield_process();

process * get_current_process();

void assign_quantum();
void unassign_quantum();

void set_superlock();
void unset_superlock();

#endif
