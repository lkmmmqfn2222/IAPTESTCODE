#include "bsp.h"
#include "delay.h"

#define APPLICATION_ADDRESS   (uint32_t)0x08000000 

int main()
{
	
	SCB->VTOR = 0x08008000;
	
	typedef  void(*pFunction)(void);
	pFunction Jump_To_Application;
	uint32_t JumpAddress;
	
	LED_Init();
	delay_init();
	for (int i=0;i<=5;i++)
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);
		delay_ms(500);
		GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_RESET);
		delay_ms(500);
	}
	while (1)
	{
		delay_ms(1000);
		JumpAddress = *(__IO uint32_t*)(APPLICATION_ADDRESS + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);	
		Jump_To_Application();
		
	}
}
