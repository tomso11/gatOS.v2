#include "stdio.h"
#include "stdioASM.h"
#define LETTER  0
#define FORMAT  1
#define NULL 0
#define BUFFERSIZE 100


void intToString(int n, char*buffer, int digits)
{
	*buffer = ((n/10)%10)+'0';
	*(buffer+1)=(n%10)+'0';
	*(buffer+2)=0;
}


void printProcesses()
{
	sys_call(10, (qword)NULL, (qword)NULL , (qword)NULL);
}

void removeProcess(int pid)
{
	sys_call(11, (qword)pid, (qword)NULL , (qword)NULL);
}

void lockMutex(int mut)
{
	sys_call(20 , (qword)mut , (qword)NULL, (qword)NULL);
}

void freeMutex(int mut)
{
	sys_call(21 , (qword)mut , (qword)NULL, (qword)NULL);
}

int getMutex(char * name)
{
	return sys_call(19 , (qword)name, (qword)NULL, (qword)NULL);
}

int createProcess(void * entry  , char * name, void * args)
{
	return (int)sys_call(23, (qword)entry, (qword)name, (qword)args);
}
void wait(int semaphore)
{
	sys_call(25, (qword)semaphore , (qword)NULL, (qword)NULL );
}

void signal(int semaphore)
{
	sys_call(15, (qword)semaphore , (qword)NULL,(qword)NULL );
}

int semOpen(char * name)
{
	return (int)sys_call(16, (qword)name , (qword)NULL,  (qword)NULL);
}

int semCreate(char * name , int start)
{
	return (int)sys_call(17, (qword)name , (qword)start, (qword)NULL);
}

void semClose(int index)
{
	sys_call(18, (qword)index , (qword)NULL, (qword)NULL);
}

void sleep(int time)
{
	sys_call(24, (qword)time , (qword)NULL,  (qword)NULL);
}

int putchar(char c){
	char  msg ;
	msg = c;
	writeC(&msg,1);
	return 0;
}

char getchar(){
	char  buffer;
	buffer = 0;
	while( buffer == 0){
		readC(&buffer,1);
	}

	return buffer;
}


void putNumber(int n){
	char * buffer = malloc(20);
	int size = 0;
	if(n < 0){
		putchar('-');
		n = -n;
	}
	while(n/10){
		*buffer = n%10 +'0';
		buffer++;
		size++;
		n = n/10;
	}
	*buffer = n + '0';
	size++;
	while(size != 0){
		putchar(*buffer);
		buffer--;
		size--;
	}

}

void print(char * s){
	while(*s!=0){
		putchar(*s);
		s++;
	}
}


void printFF(const char * format, char ** s, int * n){
 int state = LETTER;
 while(*format != 0){
 	switch(state){

		case LETTER:
			if(*format == '%'){
				state = FORMAT;
				format++;
				break;
			}

			putchar(*format);
			format++;
			break;
		case FORMAT:
			if(*format == 'd'){
				if(n != NULL){
					int nummber = *n;
				n++;
				putNumber(nummber);
				state = LETTER;
				format++;
				break;

				}

			}
			if(*format == 's'){
				if(s != NULL){
					char * string = s[0];
				s++;
				while(*string != 0){
					putchar(*string);
					string++;
				}
				state = LETTER;
				format++;
				break;

				}

			}
			else {
				putchar('%');
				format++;
				break;
			}

		}
	}
	if(*(format-1) == '%'){
		putchar('%');
	}
	return;

}

int scanFF(const char * format, char ** s, int * n){
 int bufferindex;
 int state = LETTER;
 int indexS = 0;
 while(*format != 0){
 	switch(state){

		case LETTER:
			if(*format == '%'){
				state = FORMAT;
				format++;
				break;
			}
			char c;
			while((c=getchar()) == *format){
				;
			}
			format++;
			break;
		case FORMAT:
			if(*format == 'd'){
				if(n != NULL){
					int nummber = *n;
				n++;
				putNumber(nummber);
				state = LETTER;
				format++;
				break;

				}

			}
			if(*format == 's'){
				if(s != NULL){
				char * string = s[indexS];
				indexS++;
				char c;
				bufferindex = 0;
				while((c = getchar()) != '\n'){
					if(c == '\b'){
						if(bufferindex != 0){
							string--;
							bufferindex--;
						}
					}
					else{
						if(bufferindex == BUFFERSIZE){
							break;
						}
						*string = c;
						string++;
						bufferindex++;
					}
				}
				*string = 0;
				state = LETTER;
				format++;
				break;

				}

			}
		}
	}
	return bufferindex == BUFFERSIZE;

}
	static char * position = (char*)0x1000000;

void * malloc(long int bytes){
	return (char *)sys_call(13,bytes,0, NULL);
}
char * getPosition(){return position;}

int strcmp(char * str1, char * str2){
	while((*str1 != 0) && (*str2 != 0)){
		if((*str1 - *str2) > 0){
			return 1;
		}
		if((*str1 - *str2) < 0){
			return -1;
		}
		str1++;
		str2++;
	}
	if(*str1 == 0 && *str2 == 0){
		return 0;
	}
	if(*str1 == 0){
		return -1;
	}
	return 1;
}

int strcmpN(char * str1, char * str2,int number){
	while((*str1 != 0) && (*str2 != 0) && number){
		if((*str1 - *str2) > 0){
			return 1;
		}
		if((*str1 - *str2) < 0){
			return -1;
		}
		str1++;
		str2++;
		number--;
	}
	if(!number){
		return 0;
	}
	if(*str1 == 0 && *str2 == 0){
		return 0;
	}
	if(*str1 == 0){
		return -1;
	}
	return 1;
}
