#ifndef STDIO_H
#define STDIO_H

#include "stdioASM.h"
#define NULL 0

int putchar(char c);
char getchar();
void printFF(const char * format, char ** s, int * n);
void putNumber(int n);
void * malloc(long int bytes);
int scanFF(const char * format, char ** s, int * n);
int strcmpN(char * str1, char * str2,int number);
int strcmp(char * str1, char * str2);
void print(char * s);
int createProcess(void * entry  , char * name, void * args);
void wait(int semaphore);
void signal(int semaphore);
int semOpen(char * name);
int semCreate(char * name , int start);
void semClose(int index);
void sleep(int time);
void removeProcess(int pid);
int getMutex(char * name);
void lockMutex(int mut);
void freeMutex(int mut);
void intToString(int n, char*buffer, int digits);

void printProcesses();

#endif
