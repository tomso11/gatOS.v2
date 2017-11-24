#ifndef STDIO_H
#define STDIO_H


#include "stdioASM.h"
#include "types.h"
int putchar(char c);
char getchar();
void print(char * s);
void printFF(const char * format, char ** s, int * n);
void putNumber(int n);
void * malloc(long int bytes);
int scanFF(const char * format, char ** s, int * n);
int strcmpN(char * str1, char * str2,int number);
int strcmp(char * str1, char * str2);

#endif