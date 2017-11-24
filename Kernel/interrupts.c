#include "types.h"
#include <video.h>
#include <lib.h>
#include <interrupts.h>
#include <rtc.h>
#include <keyboard.h>
#include <mouse.h>
#include <naiveConsole.h>
#include <moduleLoader.h>
#include <scheduler.h>



#define MAX_LISTENERS 50

void io_wait();
static void * const shellAddress = (void*)0xC00000;
static void * const currentAddress = (void*)0x800000;

static int sleepListeners =0;
static int auxj=0;


static int sleepPIDS[MAX_LISTENERS];
static int sleepCounter[MAX_LISTENERS];
static int alarmSleep[MAX_LISTENERS];


#pragma pack(push)
#pragma pack(1)
typedef struct IDT_Entry {
  	uint16_t offset_l; //bit 0..15
	uint16_t selector;
	uint8_t zero_l;
	uint8_t attrs;
	uint16_t offset_m; //16..31
	uint32_t offset_h; //32..63
	uint32_t zero_h;
}IDT_Entry;

typedef int (*EntryPoint)();
typedef int (*EntryPointS)(int);

#pragma pack(pop)

static IDT_Entry * IDT = (IDT_Entry*) 0x00;

void setIDTEntry(uint64_t offset, int entry)
{
	IDT[entry].offset_l = (uint16_t)offset & 0xFFFF;
	IDT[entry].offset_m = (uint16_t)(offset >> 16) & 0xFFFF;
	IDT[entry].offset_h = (uint16_t)(offset >> 32) & 0xFFFFFFFF;
	IDT[entry].zero_l = 0;
	IDT[entry].zero_h = 0;
	IDT[entry].attrs = 0x8E;
	IDT[entry].selector = 0x08;

}



void loadIDT()
{

	setIDTEntry((uint64_t) timerTickHandler,0x20);
	setIDTEntry((uint64_t) master,0x23);
	setIDTEntry((uint64_t) master,0x24);
	setIDTEntry((uint64_t) master,0x25);
	setIDTEntry((uint64_t) master,0x26);
	setIDTEntry((uint64_t) master,0x27);
	setIDTEntry((uint64_t) slave,0x28);
	setIDTEntry((uint64_t) slave,0x29);
	setIDTEntry((uint64_t) master,0x2A);
	setIDTEntry((uint64_t) master,0x2B);
	setIDTEntry((uint64_t) master,0x2D);
	setIDTEntry((uint64_t) master,0x2E);

	setIDTEntry((uint64_t) generalProtectionHandler,0x0D);
	setIDTEntry((uint64_t) pageFaultHandler,0x0E);
	setIDTEntry((uint64_t) sys_callHandler,0x80);
	setIDTEntry((uint64_t) keyboardHandler,0x21);
	setIDTEntry((uint64_t) mouse_handler,0x2C);

}




void enablePIC()
{

	outputb(0x21,0xF8);
	outputb(0xA1,0xEF);

}

void io_wait()
{

	int time = 100000;
	while(time--);

}

void spure(){
	printMsg(10,10,"xxxxxxxxxx",0x20);
}


void timerTickHandlerC()
{
	char time[9];
	getTime(time);
	printMsg(2,0,time,0x20);

	for(auxj=0;auxj < sleepListeners; auxj++){
		sleepCounter[auxj]+=1;
		if(sleepCounter[auxj] == alarmSleep[auxj]){
			doneSleeping(auxj);
            auxj--;
		}
	}
}
void deleteSleep(int index){
	if(index >= sleepListeners) return;

	alarmSleep[index] = alarmSleep[sleepListeners-1];
	sleepPIDS[index] = sleepPIDS[sleepListeners-1];
	sleepCounter[index] = sleepCounter[sleepListeners-1];
    sleepListeners--;


}

void addSleep(int pid,int interval){
	if(sleepListeners >= MAX_LISTENERS) {
		ncPrint("_______________");
		return;
	}

	alarmSleep[sleepListeners] = interval;
	sleepPIDS[sleepListeners] = pid;
	sleepCounter[sleepListeners] = 0;
	sleepListeners++;
}

void doneSleeping(int index){

	changeProcessState(sleepPIDS[index],READY);
	deleteSleep(index);

}

void generalProtectionHandlerC(){

	ncPrint("RECOVERING GENERAL PROTECTION FAULT");

	int pid = getCurrentPid();

	removeProcess(pid);

	printMsg(20,0,"General Protection", 0x11);

}

void pageFaultHandlerC(){

	ncPrint("RECOVERING FROMPAGE FAULT");

	sys_call_runC((qword) 4, (qword)0, (qword)0,(qword) 0, (qword)0, (qword)0);


}

void sleep(int time){
	int myPid=getCurrentPid();
    addSleep(myPid,time);
	changeProcessState(myPid,SLEEPING);
    _yield();
	return;
}
