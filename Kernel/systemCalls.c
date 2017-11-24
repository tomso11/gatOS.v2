#include <stdint.h>
#include <terminal.h>
#include <video.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <scheduler.h>
#include <systemCalls.h>
#include "malloc.h"
#include "process.h"
#include "interrupts.h"
#include "mutex.h"
#include "threads.h"
#include "semaphores.h"

#define DUMMY  1
#define EDITOR 2
#define FORTUNE 3
#define SHELL 4
#define PAGE_FAULT_MODULE 5
#define SYSTEM_CALL_COUNT 35

static void * const dummyAddress = (void*)0xA00000;
static void * const shellAddress = (void*)0xC00000;
static void * const currentAddress = (void*)0x800000;
static void * const editorAddress = (void*)0xE00000;
static void * const fortuneAddress = (void*)0x600000;
static void * const sampleCodeModuleAddress = (void*)0x400000;
typedef int (*EntryPoint)(int i);

typedef qword (*sys)(qword rsi, qword rdx, qword rcx, qword r8, qword r9);
static sys sysCalls[SYSTEM_CALL_COUNT];

//escribe en el file descriptor que le pasen.
//solo tiene implementado para imprimir por salida estandar
qword sys_call_writeC(qword qstdout, qword qmsg, qword qlength, qword rcx, qword r8, qword r9){
	uint32_t stdout = (uint32_t) qstdout;
	char * msg = (char*) qmsg;
	int length = (int) qlength;
	if(stdout == 0){
		while(length != 0){
			if(*msg == '\n'){
				lineJump();
			}
			else{
				putChar(*msg);
			}
			msg++;
			length--;
		}
	}
	return 0;

}
//lee del file descriptor que le pasen.
//solo tiene implementado para leer de la entrada estandar
qword sys_call_readC(qword qstdin, qword qbuffer, qword qlength, qword rcx, qword r8, qword r9){

	uint32_t stdin = (uint32_t) qstdin;
	char * buffer = (char*) qbuffer;
	int length = (int) qlength;
	if(stdin == 0){
		return getTerminalBuffer(buffer, length);
	}
	//no leyo nada
	return 0;
}

qword sys_call_clearC(qword rdi,qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	int i;
	for(i=25; i>2;i--){
		clearRow(i);
	}
	setCursor(3,0);
	return 0;
}

qword sys_call_echoC(qword qon,qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	int on = (int) qon;
	if(on){
		echoON();
		return 0;
	}
	echoOF();
	return 0;

}

qword sys_call_runC(qword qprogram, qword dumyProgram, qword rdx, qword rcx, qword r8, qword r9){
	resetBuffer();
	int program = (int) qprogram;
	void * moduleAdress;
	switch(program){

		case DUMMY:

			 moduleAdress = dummyAddress;
			 mapModulesLogical(moduleAdress);
			 updateCR3();
			 createProcess(dummyAddress, "dummy" , (void*)dumyProgram);
			break;
		case EDITOR:
			moduleAdress = editorAddress;
			mapModulesLogical(moduleAdress);
			updateCR3();
			createProcess(editorAddress, "editor" , NULL);
			break;

		case FORTUNE:

			moduleAdress = fortuneAddress;
			mapModulesLogical(moduleAdress);
			updateCR3();
			createProcess(fortuneAddress, "fortune" , NULL);
			break;
		case SHELL:
			moduleAdress= shellAddress;
			mapModulesLogical(moduleAdress);
			updateCR3();
			createProcess(shellAddress, "Shell" , NULL);
			break;
		case PAGE_FAULT_MODULE:
			 moduleAdress = sampleCodeModuleAddress;
			 mapModulesLogical(moduleAdress);
			 updateCR3();
			 int pid = createProcess(sampleCodeModuleAddress, "pagefaul" , NULL);
			 Process * p = getProcessById(pid);
			 Thread * t = p->thread[0];
			 t->userStack->cs = 0x00D;
			break;
		default:
			ncPrint("MUERTE");
	}

	int pid = getCurrentPid();

	removeProcess(pid);

	// updateCR3();
	return (qword)currentAddress;

	//mapModulesLogical(shellAddress);
	//updateCR3();
	//((EntryPoint)currentAddress)();

}

qword sys_call_getAddressOfModuleC(qword qprogram, qword rsi, qword rdx, qword rcx, qword r8, qword r9){


	int program = (int) qprogram;
	void * moduleAdress;
	switch(program){

		case DUMMY:
			 //createProcess(dummyAddress, "dummy");
			 moduleAdress = dummyAddress;
			break;
		case EDITOR:
			//createProcess(editorAddress, "editor");
			moduleAdress = editorAddress;
			break;

		case FORTUNE:
			//createProcess(fortuneAddress, "fortune");
			moduleAdress = fortuneAddress;
			break;
		case SHELL:
			moduleAdress= shellAddress;
			break;

	}
	// mapModulesLogical(moduleAdress);
	// updateCR3();
	//((EntryPoint)moduleAdress)();
	return (qword)moduleAdress;

	//mapModulesLogical(shellAddress);
	//updateCR3();
	//((EntryPoint)currentAddress)();

}

qword sys_call_printProcesses(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9){
	printProcesses();
	return 0;
}

qword sys_call_changeModuleEnvironmetC(qword qi, qword qj,qword rdx, qword rcx, qword r8, qword r9){
	int i = (int) qi;
	int j = (int) qj;
	changeStartModule(i,j);
	return 0;
}

qword sys_call_undoBackwardsC(qword qfrom, qword rsi,qword rdx, qword rcx, qword r8, qword r9){
	int from = (int) qfrom;
	changeStopBackwards(from);
	return 0;
}

qword sys_call_kill(qword qpid, qword rsi,qword rdx, qword rcx, qword r8, qword r9){
	int pid = (int) qpid;
	removeProcess(pid);
	return 0;
}


qword sys_call_createThread(qword qentryPoint, qword qargs,qword rdx, qword rcx, qword r8, qword r9)
{
	void * entryPoint = (void *)qentryPoint;
	void * args = (void *)qargs;
	Process * p = getCurrentProcess();
	createThread(entryPoint, args, p);
	return 0;
}

qword sys_call_mallock(qword qnumberOfBytes, qword rsi,qword rdx, qword rcx, qword r8, qword r9){
	size_t numberOfBytes = (size_t) qnumberOfBytes;
	return (qword) mallock(numberOfBytes);
}

qword sys_call_createProcess(qword qentryPoint , qword description, qword param,qword rcx, qword r8, qword r9)
{
	void * entryPoint = (void *)qentryPoint;
	return createProcess(entryPoint , (char*)description , (void*)param);
}


qword sys_call_sleep(qword time,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	sleep((int)time);
	return 0;
}


//SysCall-Sync


qword sys_call_wait(qword semaphore,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	wait((int)semaphore);
	return 0;
}

qword sys_call_signal(qword semaphore,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	signal((int)semaphore);
	return 0;
}

qword sys_call_semOpen(qword name,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	return semOpen((char*)name);
}

qword sys_call_semCreate(qword name , qword start,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	return semCreate((char*)name,(int)start);
}

qword sys_call_semClose(qword index,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	semClose((int)index);
	return 0;
}


qword sys_call_getMutex(qword mutexName,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	return getMutex((char*)mutexName);
}

qword sys_call_lockMutex(qword mutex,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	lockMutex((int)mutex);
	return 0;
}

qword sys_call_freeMutex (qword mutex,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	freeMutex((int)mutex);
	return 0;
}

qword sys_call_closeMutex(qword index,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	closeMutex((int)index);
	return 0;
}


void setUpSystemCalls(){

	sysCalls[4] = (sys)&sys_call_writeC;
    sysCalls[3] = (sys)&sys_call_readC;
    sysCalls[5] = (sys)&sys_call_clearC;
    sysCalls[6] = (sys)&sys_call_echoC;
    sysCalls[7] = (sys)&sys_call_runC;
    sysCalls[10] = (sys)&sys_call_printProcesses;
    sysCalls[8] = (sys)&sys_call_changeModuleEnvironmetC;
    sysCalls[9] = (sys)&sys_call_undoBackwardsC;
    sysCalls[11] = (sys)&sys_call_kill;
    sysCalls[12] = (sys)&sys_call_createThread;
    sysCalls[13] = (sys)&sys_call_mallock;
    sysCalls[14] = (sys)&sys_call_getAddressOfModuleC;
    sysCalls[15] = (sys)&sys_call_signal;
    sysCalls[16] = (sys)&sys_call_semOpen;
    sysCalls[17] = (sys)&sys_call_semCreate;
    sysCalls[18] = (sys)&sys_call_semClose;
    sysCalls[19] = (sys)&sys_call_getMutex;
    sysCalls[20] = (sys)&sys_call_lockMutex;
    sysCalls[21] = (sys)&sys_call_freeMutex;
    sysCalls[22] = (sys)&sys_call_closeMutex;
    sysCalls[23] = (sys)&sys_call_createProcess;
    sysCalls[24] = (sys)&sys_call_sleep;
    sysCalls[25] = (sys)&sys_call_wait;
		sysCalls[26] = (sys)&sys_call_openPipe;
		sysCalls[27] = (sys)&sys_call_createPipe;
		sysCalls[28] = (sys)&sys_call_closePipe;
		sysCalls[29] = (sys)&sys_call_read;
		sysCalls[30] = (sys)&sys_call_write;
}

qword sys_call_openPipe(qword name,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	 return (qword)openPipe((char *)name);
}
qword sys_call_createPipe(qword name,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	return (qword)createPipe((char *)name);
}
qword sys_call_closePipe(qword pipe,qword rsi,qword rdx, qword rcx, qword r8, qword r9)
{
	closePipe((Pipe *)pipe);
	return 0;
}
qword sys_call_read(qword pipe,qword result,qword bytes, qword rcx, qword r8, qword r9)
{
	return (qword)read((Pipe *)pipe , (char*)result,(int) bytes);
}
qword sys_call_write(qword pipe,qword message,qword rdx, qword rcx, qword r8, qword r9)
{
	return (qword)write((Pipe *)pipe , (char*) message);
}
qword syscallHandler(qword rdi,qword rsi, qword rdx, qword rcx, qword r8, qword r9){


    if(rdi < 0 || rdi >= SYSTEM_CALL_COUNT) {
        return 0;
    }
    return sysCalls[rdi](rsi,rdx,rcx,r8,r9);
}

void printrsi(qword rsi){
	ncPrintHex(rsi);
	while(1);
}
