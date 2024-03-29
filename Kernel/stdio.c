#include "stdio.h"
#include "stdioASM.h"
#include <naiveConsole.h>
#include "malloc.h"

#define LETTER  0
#define FORMAT  1
#define BUFFERSIZE 100


int putchar(char c){
	char msg = c;
	writeC(&msg,1);
	return 0;
}

char getchar(){
	char  buffer ;
	buffer = 0;
	while( buffer == 0){
		readC(&buffer,1);
	}

	return buffer;
}


void putNumber(int n){
	char * buffer = kmalloc(20);
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
	return mallock(bytes);
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

int strcpy(char * to, const char * from) {
	int i;
	for (i = 0; from[i] != '\0'; i++)
		to[i] = from[i];
	to[i] = '\0';
	return i;
}

/* Copia en str los valores ascii de los digitos de value en la base indicada.
** Devuelve la cantidad de digitos copiados. */
int itoa(int value, char *str, int base) {
	char *p = str;
	char *p1, *p2;
	int len = 0;

	if(value < 0 && base == 10) {
		value = -value;
		len++;
		*p++ = '-';
		str++;
	}

	//Calculate characters for each digit
	do
	{
		int remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		len++;
	}
	while (value /= base);

	// Terminate string in str.
	*p = '\0';

	//Reverse string in str.
	p1 = str;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return len;
}
