#ifndef VIDEO_H
#define VIDEO_H

void printMsg(int f, int c, char*msg,  char color);
void printMsgCursor( char*msg);
void clear();
void matrix();
void printChar(int f, int c, char a, char color);
char getChar(int f, int c);
void udrawMouse(int f, int c);
void drawMouse(int f, int c);
void putChar( char a ) ;
void forwardCursor();
void forwardCursorB();
void scroll();
void backwardCursor();
int validPosition(int f, int c);
void lineJump();
void printMsgCursor( char * msg);
void clearPosition(int f, int c);
void clearRow(int f);
void setCursor(int f, int c);
int getScreen(int f, int c);
void updateScreen();
void copyRow(int from, int to);
void selection(int finit, int cinit, int ffin, int cfin);
void changeColor(int f, int c, char color);
void undoSelection(int finit, int cinit, int ffin, int cfin);
void paste();
void copyMouse(int initial, int end);
void changeStartModule(int i, int j);
void changeStopBackwards(int from);
int isBackwardsOn();
void backInLine();

#endif



