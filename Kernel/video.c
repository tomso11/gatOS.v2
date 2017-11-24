#include <video.h>
#include <terminal.h>

#define SCREEN 0xB8000
#define BLACK 0x00
#define SPACE 0x20
#define DEFAULT 0x0F
#define WIDTH 80
#define HEIGHT 25

static char video[HEIGHT][WIDTH] = {{0}};
static char attributes[HEIGHT][WIDTH];
static int i=0; //posicion del cursor horizontal
static int j=3;  // posicion del cursor vertical

static char mouseSelect[HEIGHT*WIDTH];
static int size=0;

static int iStartModule = 0;
static int jStartModule = 3;
static int stopBackWards = -1;



void changeStartModule(int f, int c){
	if(f<0 || f>WIDTH || c < 3 || c>HEIGHT){
		return;
	}
	jStartModule = c;
	iStartModule = f;
	i=f;
}

void changeStopBackwards(int from){
	stopBackWards = from;
	return;
}

int isBackwardsOn(){
	if(stopBackWards == -1){
		return 1;
	}
	return (stopBackWards <= i);

}


void putChar(char a ) { // se define color estandar
		printChar(j,i, a ,DEFAULT );
		forwardCursor();
}
void setCursor(int f, int c){
	if(!validPosition(f,c)) return;
	i=c;
	j=f;
}
void updateScreen(){
	char * screen = (char*) SCREEN;
	for(int c=0; c<WIDTH; c++){
		for(int f=0; f<HEIGHT;f++){
			*(screen + f*WIDTH*2 + c*2)= video[f][c];
			*(screen + f*WIDTH*2 + c*2 +1)= attributes[f][c];
		}
	}
}
int getScreen(int f, int c){
	if(!validPosition(f,c)){
		return -1;
	}
	return (char)video[f][c];
}

void selection(int finit, int cinit, int ffin, int cfin){

	int i= finit*WIDTH+ cinit;
	int fin= ffin*WIDTH+cfin;
	copyMouse(i , fin);
	while(validPosition(i/WIDTH,i%WIDTH) && i<=fin ){

		drawMouse(i/WIDTH,i%WIDTH);
		i++;
	}
}

void changeColor(int f, int c, char color){

		char * screen = (char*)SCREEN;

	*(screen + f*WIDTH*2 + c*2 + 1 )=(char)color;


}

void undoSelection(int finit, int cinit, int ffin, int cfin){
	int i= finit*WIDTH+ cinit;
	int fin= ffin*WIDTH+cfin;
	while(validPosition(i/WIDTH,i%WIDTH) && i<=fin ){

		udrawMouse(i/WIDTH,i%WIDTH);
		i++;
	}

}

void forwardCursor(){

	if(i==WIDTH-1){
		if(j==HEIGHT-1){
			 scroll();
			updateScreen();
		}else{
			j++;
			i=iStartModule;
		}
	}else{
		i++;
	}

}

void lineJump(){
	if(j==HEIGHT-1) {
			scroll();
	} else{
		int aux= j +1;
		while(j!=aux){
		forwardCursor();
	}
}
}


void scroll(){
	int auxf;
	for(auxf=jStartModule+1; auxf<HEIGHT;auxf++){
		copyRow(auxf,auxf-1);
	}
	clearRow(HEIGHT-1);
	j=HEIGHT-1;
	i=iStartModule;
}
void copyRow(int from, int to){
	for(int k=0; k<WIDTH;k++){
		video[to][k]=video[from][k];
		clearPosition(from,k);
	}
}
void clearRow(int f){
	int k;
	for(k=0;k<WIDTH;k++){
		clearPosition(f,k);
	}
}
void clearPosition(int f, int c){
	if(!validPosition(f,c)) return;
	video[f][c]=' ';
}
void backwardCursor(){
	if(i==iStartModule){
		if(j!=jStartModule){
			j--;
			i=WIDTH-1;
		}
		//Llegamos a principio pantalla, nada que borrar
	}else{
		i--;

	}
}

void backInLine(){
	if(i==iStartModule){
		if(j!=jStartModule){
			return;
		}
		//Llegamos a principio pantalla, nada que borrar
	}else{
		i--;

	}
}


void printChar(int f, int c,  char a,  char color)
{
	if(!validPosition(f,c)) return;
	video[f][c] = a;
	updateScreen();

}


void drawMouse(int f, int c){
	if(!validPosition(f,c)) return;
	attributes[f][c]=(char) 0x30;
}
void udrawMouse(int f, int c){
	if(!validPosition(f,c)) return;
	 attributes[f][c]=(char)DEFAULT; // fondo negro con blanco como texto
}

int validPosition(int f, int c){
	return (f>=HEIGHT || c>=WIDTH || f<0 || c<0)? 0:1;
}

void printMsg(int f, int c,  char*msg, char color)
{
	if(!validPosition(f,c)) return;
	int i = f*WIDTH + c;
	while(*msg)
	{
		if(!validPosition(i/WIDTH,i%WIDTH)) return; // el mesaje excede el tamaño . Scroll no permitido
		printChar(i/WIDTH, i%WIDTH,*msg, 0x20);
		msg++;
		i++;
	}
}
void printMsgCursor( char * msg) // cursor position, default color
{
	while(*msg)
	{
		putChar(*msg);
		msg++;
	}
}


void clear()
{



	for(int f=0; f<HEIGHT; f++){
		for(int c=0; c<WIDTH; c ++){
			video[f][c]=' ';
			attributes[f][c] = (char)DEFAULT;
		}
	}

	updateScreen();
}

void copyMouse(int initial, int end){

	size=end-initial+1;
	int h=0;
	for(h=0; h<size; h++){
		char c=getScreen((h+initial)/WIDTH , (h+initial)%WIDTH);
		if(c==0){
			c=' ';
		}
		mouseSelect[h]=c;
	}

}

void paste(){
	for(int h=0; h<size; h++)
		putTerminalBuffer(mouseSelect[h]);
}
