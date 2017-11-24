#ifndef MOUSE_H
#define MOUSE_H

void mouse_init();
void mouse_wait(uint8_t type);
void mouse_write(uint8_t write);
uint8_t mouse_read();
void mouse_handlerC();
int detect_ps2_mouse();

#endif
