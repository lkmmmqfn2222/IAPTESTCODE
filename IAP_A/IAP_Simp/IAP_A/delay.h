#ifndef _DELAY_H
#define _DELAY_H
#include "bsp.h"
#include "misc.h"

void delay_init();
void delay_us(u32 nus);
void delay_ms(u16 nms);

#endif // !_DELAY_H
