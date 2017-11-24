#include <lib.h>
#include <rtc.h>

void getTime(char * buffer)
{
	char h, m, s;
	outputb(0x70,0x04);
	h = inputb(0x71);
	h = (( (h & 0x0F) + (((h & 0x70) / 16) * 10) ) | (h & 0x80));
	outputb(0x70,0x02);
	m = inputb(0x71);
	m = (m & 0x0F) + ((m / 16) * 10);
	outputb(0x70,0x00);
	s = inputb(0x71);
	s = (s & 0x0F) + ((s / 16) * 10);
	intToString(h,buffer,2);
	*(buffer+2)=':';
	intToString(m,buffer+3,2);
	*(buffer+5)=':';
	intToString(s,buffer+6,2);
}
