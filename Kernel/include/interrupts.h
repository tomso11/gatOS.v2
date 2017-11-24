#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"
void setIDTEntry(uint64_t offset, int entry);
void loadIDT();
void enablePIC();
void timerTickHandlerC();
void timerTickHandler();
void sti();
void keyboardHandlerC();
void keyboardHandler();
void registrer_Mouse();
void sys_callHandler();
void generalProtectionHandler();
void generalProtectionHandlerC();
void spure();
void sleep(int time);
void doneSleeping(int index);
void deleteSleep(int index);

#endif