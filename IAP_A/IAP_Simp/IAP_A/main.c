#include "bsp.h"
#include "delay.h"
#include "stdio.h"
#include "sys.h"
#include "iap.h"
#include "myprintf.h"
#include "menu.h"

#define SIMHOST 0//1:use SIMHOST for debug printf

int main()
{
	SCB->VTOR = 0x8000000;
	NVIC_Configuration();
	delay_init();
	Key_Init();
	LED_Init();
	UART_Init(115200);

	while (1)
	{
		Main_Menu();
	}
}
