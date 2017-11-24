#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <interrupts.h>
#include <keyboard.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
void intToString(char n, char*buffer, int digits);
char *cpuVendor(char *result);

void outputb(uint16_t portid, uint8_t value);
uint8_t inputb(uint16_t portid);
void timerTick();
void sti();
void keyboardHandler();
void sys_callHandler();
void sys_write();
void test();
void pageFaultHandler();
void terminalInitialize();
void cli();
void finishStartup();

void master();
void slave();
void mouse_handler();
void updateCR3();

void _yield();

uint64_t readCR2();


#endif
