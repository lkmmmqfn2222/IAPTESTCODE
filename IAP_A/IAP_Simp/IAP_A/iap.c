#include "iap.h"
#include "sys.h"
#include "bsp.h"
#include "stmflash.h"
#include "delay.h"
#include "stm32f10x_usart.h"
#include "string.h"
#include "myprintf.h"

IAPFun Jump2APP;
u16 IAPBuff[1024];

/*************************
**
**appxaddr:start address of the app
**appbuf:the code of the app
**appsize:the size of the app (byte)
**
**************************/
void IAPWriteAppbin(u32 appxaddr, u8 *appbuf, u32 appsize)
{
	u16 t;
	u16 i = 0;
	u16 temp;
	u32 fwaddr = appxaddr;//当前写入的地址
	u8 *dfu = appbuf;
	for (t = 0; t < appsize; t += 2)
	{
		temp = (u16)dfu[1] << 8;
		temp += (u16)dfu[0];
		dfu += 2;//偏移2个字节
		IAPBuff[i++] = temp;
		if (i == 1024)
		{
			i = 0;
			STMFLASH_Write(fwaddr, IAPBuff, 1024);
			fwaddr += 2048;//偏移2048  16=2*8.所以要乘以2.
		}
	}
	if (i)STMFLASH_Write(fwaddr, IAPBuff, i);//将最后的一些内容字节写进去.  
}


void iap_load_app(u32 appxaddr)
{
	if (((*(vu32*)USART_RX_BUF) & 0x2FFE0000) == 0x20000000)	//检查栈顶地址是否合法.
	{
		FLASH_Lock();
		SYS_Dinit();
		Jump2APP = (IAPFun)*(vu32*)(appxaddr + 4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr); 					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		Jump2APP(); 									//跳转到APP.
	}
}

void Jump_To_APP(uint32_t appxaddr)
{
	FLASH_Lock();
	SYS_Dinit();
	Jump2APP = (IAPFun)*(vu32*)(appxaddr + 4); 		//用户代码区第二个字为程序开始地址(复位地址)		
	__set_MSP(*(vu32*)appxaddr);   					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
	Jump2APP();
}

//
//void Serial_Download(void)
//{
//	if (((*(vu32*)(USART_RX_BUF + 4)) & 0xF000000) == 0x8000000)//0X08XXXXXX.
//	{
//		IAPWriteAppbin(FLASH_APP1_ADDR, USART_RX_BUF, Applength);  
//	}
//}


//void Serial_Upload(void)
//{
//	u8 temp;
//	USART_DeInit(USART1);
//	USART_Cmd(USART1, DISABLE);
//	delay_ms(10);
//	UART_Init(115200);
//	memset(USART_RX_BUF, 0, sizeof(u8)*USART_REC_LEN);
//	while (1)
//	{
//		delay_ms(1000);
//		myprint("Please Send Image File Inside 5S\r\n");
//		delay_ms(5000);
//		delay_ms(5000);
//		Applength = USART_RX_CNT;
//		break;
//	}
//		
//}

void Refresh_Thread(void)
{
	if (((*(vu32*)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000)// 0X08XXXXXX.
	{
		iap_load_app(FLASH_APP1_ADDR);  //Execute FLASH APP CODE
	}
	else
	{
		myprint("Flash Address Is Error!");
		delay_ms(500);
	}
}