
typedef unsigned long qword;
int readC(char * buffer, int size);
void writeC(char * msg, int size);
qword sys_call(int index, qword parametre1, qword parametre2);
void cli();

