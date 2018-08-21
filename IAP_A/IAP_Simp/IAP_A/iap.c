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
	u32 fwaddr = appxaddr;//��ǰд��ĵ�ַ
	u8 *dfu = appbuf;
	for (t = 0; t < appsize; t += 2)
	{
		temp = (u16)dfu[1] << 8;
		temp += (u16)dfu[0];
		dfu += 2;//ƫ��2���ֽ�
		IAPBuff[i++] = temp;
		if (i == 1024)
		{
			i = 0;
			STMFLASH_Write(fwaddr, IAPBuff, 1024);
			fwaddr += 2048;//ƫ��2048  16=2*8.����Ҫ����2.
		}
	}
	if (i)STMFLASH_Write(fwaddr, IAPBuff, i);//������һЩ�����ֽ�д��ȥ.  
}


void iap_load_app(u32 appxaddr)
{
	if (((*(vu32*)USART_RX_BUF) & 0x2FFE0000) == 0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{
		FLASH_Lock();
		SYS_Dinit();
		Jump2APP = (IAPFun)*(vu32*)(appxaddr + 4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr); 					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		Jump2APP(); 									//��ת��APP.
	}
}

void Jump_To_APP(uint32_t appxaddr)
{
	FLASH_Lock();
	SYS_Dinit();
	Jump2APP = (IAPFun)*(vu32*)(appxaddr + 4); 		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
	__set_MSP(*(vu32*)appxaddr);   					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
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