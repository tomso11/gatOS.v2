
#include <terminal.h>
#include <systemCalls.h>
#include <stdint.h>
#include <stdio.h>
#include <video.h>
#include <lib.h>
#include <naiveConsole.h>

#define  BUFFERSIZE 2500


static char Buffer[BUFFERSIZE];

static char * bufferConsume;
static char * bufferPosition;
static char * bufferStart;
static int bufferSize;
static int isEcho;

int isBufferFull(){
	cli();
	return ((bufferConsume == (bufferPosition+1)) || 
	(bufferConsume == (bufferStart+bufferSize) && (bufferPosition == bufferStart)));
	sti();
}
int isBufferEmpty(){
	cli();
	return bufferConsume == bufferPosition;
	sti();
}



void echoON(){
	isEcho = 1;
}

void echoOF(){
	isEcho = 0;
}

//funcion para inicializar la terminal
//esta tiene que ir antes de que lleguen las interrupciones
void terminalInitializeC(){
	bufferConsume = Buffer;
	bufferStart = Buffer;
	bufferPosition = Buffer;
	bufferSize = BUFFERSIZE;
	isEcho = 1;

}

//pone el char que le pasan en el buffer, si este
//esta lleno lo ignora
void putTerminalBuffer(char c){

	
	//bufferConsume nunca alcanza a bufferPosition por las implementaciones
	//del Kernel asi que si es al revez es Por que hay OverFLow e ignoro las que
	//vienen
	if(isBufferFull()){
		sti();
		return ; //el buffer esta lleno
	}
	cli();
	*bufferPosition = c;
	if(c == '\b'){
		backInLine();
		putChar(' ');
		backInLine();
	}
	else if(c == '\n'){
		lineJump();
		
	}
	else if(isEcho){
		putChar(*bufferPosition);
		
	}
	bufferPosition++;
	if(bufferPosition == (bufferStart + bufferSize)){
			bufferPosition = bufferStart;
		}
	sti();
	
}

int getTerminalBuffer(char * buffer, uint32_t length){
	cli();
	int charsConsume = 0;
	while(length != 0 ){
			if(isBufferEmpty()){
				sti();
				return charsConsume;
			}
			*buffer = *bufferConsume;
			buffer++;
			charsConsume++;
			bufferConsume++;
			length--;
			if(bufferConsume == bufferStart + bufferSize){
				bufferConsume = bufferStart;
			}	
		}
	sti();
	return charsConsume;
}

void resetBuffer(){
	bufferPosition = bufferStart;
	bufferConsume = bufferStart;
}


