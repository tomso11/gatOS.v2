#include <video.h>
#include <lib.h>
#include <mouse.h>
#include <interrupts.h>


// Ver los siguientes links:
//link1: houbysoft.com : http://houbysoft.com/download/ps2mouse.html
//link2:  stevej:  http://github.com/stevej/osdev/blob/master/kernel/devices/mouse.c

uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];


void mouse_init()
{
	mouse_wait(1); //Funcion que espera a que el mouse este listo para recibir/enviar comandos/datos.
	outputb(0x64,0xA8); //Enciende el mouse auxiliar. Esto se hace enviando el comando 0xA8 al puerto 0x64
	mouse_wait(1);
	outputb(0x64,0x20); //Enviar 0x20 al puerto 0x64 para avisar que queremos que nos envie el bit de status, para luego poder modificarlo.
	uint8_t status_byte; //Aca guardamos el bit de status para poder modificarlo
	mouse_wait(0);
	status_byte = (inputb(0x60) | 2); //Aca leemos el bit de estado el puerto 60, y haciendo un OR con 2, seteamos el primer bit en 1 para poder empezar a aceptar interrupciones por la IRQ12
		
	mouse_wait(1);
	outputb(0x64,0x60); //Le avisamos al controlador que vamos a enviar un byte de estado;
	mouse_wait(1);
	outputb(0x60, status_byte);
	mouse_write(0xF6); //0xF6 es la configuracion por defecto del mouse.
	mouse_read(); //Esperamos que el mouse acepte el comando
	mouse_write(0xF4); //Encendemos el mouse
	mouse_read();



}

void mouse_wait(uint8_t type)
{
	uint32_t timeout = 100000;
	if(type==0)
	{
		while(--timeout)
		{
			if((inputb(0x64) & 0x01) == 1)
			{
				return;
			}
		}
		return;
	}
	else
	{
		while(--timeout)
		{
			if((inputb(0x64) & 0x02)==0)
				{
					return;
				}
		}
		return;
	}

}

int detect_ps2_mouse()
{
	unsigned char tmp = mouse_read();
	if(tmp!=0xFA)
	{
		return 0;	
	}
	else
	{
		return 1;	
	}

}

void mouse_write(uint8_t write)
{
	mouse_wait(1);
	outputb(0x64,0xD4);
	mouse_wait(1);
	outputb(0x60, write);
}

uint8_t mouse_read()
{
	mouse_wait(0);
	char t = inputb(0x60);
	return t;
}

static int flag=0;
static int x=10;
static int y=10;
static int xinit=0;
static int yinit=0;
static int xfin=0;
static int yfin=0;
static int selecting=0;
void mouse_handlerC()
{
	uint8_t status = inputb(0x64);
	while(status & 0x01)
	{
		int8_t mouse_in = inputb(0x60);
		if(status & 0x20)
		{
			
			switch(mouse_cycle)
			{
				case 0:
						mouse_byte[0] = mouse_in;
						if(!(mouse_in & 0x08)) return;
						++mouse_cycle;
						break;
				case 1:
						mouse_byte[1] = mouse_in;
						++mouse_cycle;
						break;
				case 2:
						mouse_byte[2] = mouse_in;
						if(mouse_byte[0] & 0x80 || mouse_byte[0] & 0x40)
						{
							mouse_cycle = 4;
							break;
						}
						if(mouse_byte[0] & 0x01)
						{
							if(selecting){
								if(yfin<yinit || ((yfin==yinit) &&(xfin<xinit))){
									undoSelection( yfin, xfin,yinit, xinit);

								}else{
									undoSelection(yinit, xinit, yfin, xfin);
								}
							xfin=x;
							yfin=y;
								
							
							}else{
								selecting=1;
								xinit=x;
								yinit=y;
								xfin=x;
								yfin=y;
							}
							if(yfin<yinit ||( (yfin==yinit) &&(xfin<xinit))){
								selection( yfin, xfin,yinit, xinit);
							
							}else{
								selection(yinit, xinit, yfin, xfin);
							}
							
						}else{
							if(selecting){
								if(yfin<yinit || ((yfin==yinit) &&(xfin<xinit))){
									undoSelection( yfin, xfin,yinit, xinit);

							}else{
									undoSelection(yinit, xinit, yfin, xfin);
							}
							selecting=0;
							updateScreen();

						}
					}
						
						if(mouse_byte[0] & 0x02)
						{
							flag=1;

						}else if(flag==1){
							paste();
							flag=0;
						}
						if(mouse_byte[0] & 0x04)
						{

							//printMsg(4,0,"Middle Click",0x20);
						}
						int8_t d= mouse_byte[0];
						int rel_x=0;
						int rel_y=0;	
						if((d&0x10)==0x10){
						rel_x=(mouse_byte[1] | 0xFFFFFF00);
						}else{
							rel_x=mouse_byte[1];
						}
						if((d&0x20)==0x20){
						rel_y=(mouse_byte[2] | 0xFFFFFF00);
						}else{
							rel_y=mouse_byte[2];
						}
						udrawMouse(y,x);
						if(x+rel_x/10 < 80 && x+rel_x/10>=0)x+=rel_x/10;
						if(y-rel_y/10 < 25 && y-rel_y/10>=0)y-=rel_y/10;
						drawMouse(y,x);
						updateScreen();

						
					
						mouse_cycle = 4;
						break;
			}
		}
		status = inputb(0x64);
		if(mouse_cycle>=4){
			mouse_cycle=0;
			return;

		} 
		
	}

}
