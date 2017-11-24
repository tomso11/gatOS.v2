#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <video.h>
#include <rtc.h>
#include <keyboard.h>
#include <interrupts.h>
#include <mouse.h>
#include <terminal.h>
#include <scheduler.h>
#include "IPC.h"
#include "buddyAllocator.h"
#include "mutex.h"
#include "systemCalls.h"
#include "malloc.h"
#include "philosophers.h"
#include "prodcons.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

int init();

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const fortuneAddress = (void*)0x600000;
static void * const dummyAddress = (void*)0xA00000;
static void * const shellAddress = (void*)0xC00000;
static void * const editorAddress = (void*)0xE00000;
static void * const currentAddress = (void*)0x800000;

void processA();
void processB();

void test1();
void test2();

void lock();
void freeLock();

void testArgs(void (* entry) (void));

void printCheckPoint();




typedef int (*EntryPoint)();
typedef int (*EntryPointS)(int);
void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

int init(){
	while(1);
}

void * initializeKernelBinary()
{
	char buffer[10];
	initializeKernelHeap();
	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress, dummyAddress, shellAddress, editorAddress, fortuneAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();
	ncPrintHex((uint64_t) endOfKernelBinary);

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

int main()
{

	clear();

	cli();
	initializeKernelHeap();
	setUpSystemCalls();
	terminalInitializeC();
	loadIDT();
	mouse_init();
	enablePIC();
	ncPrint("SetUp Kernel: OK");
	mapModulesLogical((void*)shellAddress);
	updateCR3();
	ncPrint("Starting SHELL: OK");
	resetBuffer();
	ncPrint("Reset Buffer: OK");
	createHeap();
	ncPrint("Maping Memory: OK");

	initializeMutexes();
	ncPrint("Initializing Mutex: OK");

	initializeSemaphores();
	ncPrint("Initializing Semaphores: OK");
	ncPrint("FinishStartup: OK");
	clear();
	printMsg(0,0,"Sistemas Operativos",0x0F);
	printMsg(1,0,"La hora en UTC es:",0x0F);
	//Scheduler
	createProcess(init, "init" , NULL);
	createProcess(currentAddress, "SHELL" , NULL);

	finishStartup();

	while(1);



}
