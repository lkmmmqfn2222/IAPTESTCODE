#include "menu.h"
#include "myprintf.h"
#include "bsp.h"
#include "iap.h"
#include "delay.h"

void Main_Menu(void)
{
	u8 Key = 0;
	u8 Step = 0;
	myprint("\r\n======================================================================");
	myprint("\r\n=              (C) COPYRIGHT 2018 Cairne's Studio	                  =");
	myprint("\r\n=                                                                    =");
	myprint("\r\n=  STM32F1xx In-Application Programming Application  (Version 1.0.0) =");
	myprint("\r\n=                                                                    =");
	myprint("\r\n=                            By Cairne						          =");
	myprint("\r\n======================================================================");
	myprint("\r\n\r\n");
	while (1)
	{
		if (Step == 0)
		{
			myprint("\r\n================== Main Menu ============================\r\n\n");
			myprint("  Download Image To the STM32F10x Internal Flash ------- 1\r\n\n");
			myprint("  Upload Image From the STM32F10x USART1---------------- 2\r\n\n");
			myprint("  Execute The New Program ------------------------------ 3\r\n\n");
			Step++;
		 
		}
		else if (Step==1)
		{
			Key = Get_Key();
			if (Key == 0x33)
			{
				myprint("Start to Download Image to STM32F10x Internal Flash-----\r\n");
				Serial_Download();
				Key = 0;
				delay_ms(1000);
				myprint("Download Finished!---------------------------------------\r\n");
			}
			else if (Key == 0x32)
			{
				Serial_Upload();
				myprint("Upload Image Finished,The Image Length Is %d ----------\r\n", Applength);
				Key = 0;
			
			}
			else if (Key==0x34)
			{
				Step = 0;
				Key = 0;
				Refresh_Thread();
			}
		}	 
	}		
}


