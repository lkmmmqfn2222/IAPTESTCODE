#include "bsp.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_flash.h"
#include "misc.h"
#include "delay.h"
#include "sys.h"


/**********************
 **
 **KEY1 PC13
 **KEY0 PC1
 **ACTIVE LEVEL IS LOW
 **
 ***********************/
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**********************************
 **
 **mode 0: no continuously key down
 **return 0: no key
 **return 1: key 0
 **return 2: key 1
 **
 ***********************************/

u8 Key_Scan(u8 mode)
{
	static u8 KeyFlag = 1;
	if (mode)
	{
		KeyFlag = 1;
	}
	if (KeyFlag&&(KEY0==0||KEY1==0))
	{
		delay_ms(10);//JITTER
		KeyFlag = 0;
		
		if(KEY0==0)
		{
			return 1;
		}
		else if(KEY1==0)
		{
			return 2;
		}
	}
	return 0;
}

/******************************
 **
 **UART RX DATA ADDRESS 0X20001000
 **RX_STA     : RX STATE FLAG
 **RX_CNT     : RX COUNT NUMBER
 **RX BITRATE : BOUND
 **
 **UART_TX    : PA9
 **UART_RX    : PA10
 **
 ******************************/
//u8 USART_RX_BUF[USART_REC_LEN] __attribute__ ((at(0X20003000))) =  { 0 };
//u8 __attribute__ ((section(".USARTRxBuff"))) USART_RX_BUF[USART_REC_LEN];
u8 USART_RX_BUF[USART_REC_LEN];
u16 USART_RX_STA = 0;
u16 USART_RX_CNT = 0;	
void UART_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void)
{
	u8 ResTemp;
	if (USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{
		ResTemp = USART_ReceiveData(USART1);
		if (USART_RX_CNT<USART_REC_LEN)
		{
			USART_RX_BUF[USART_RX_CNT] = ResTemp;
			USART_RX_CNT++;
		}
	}
}

void SYS_Dinit(void)
{
	RCC_DeInit();
	USART1->CR1 = 0x0;
	USART1->CR2 = 0x0;
	USART1->CR3 = 0x0;
	USART3->CR1 = 0x0;
	USART3->CR2 = 0x0;
	USART3->CR3 = 0x0;
	USART_Cmd(USART1, DISABLE);
	USART_Cmd(USART3, DISABLE);
	USART_DeInit(USART1);
	USART_DeInit(USART3);
	
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOD);
	GPIO_DeInit(GPIOE);
	GPIO_DeInit(GPIOF);
//	WFI_SET(); 		//执行WFI指令
//	INTX_DISABLE(); //关闭所有中断
//	INTX_ENABLE(); 	//开启所有中断
}


void LED_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);	 //使能PB,PE端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_2; 				 //LED0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	GPIO_SetBits(GPIOA, GPIO_Pin_8);						 //PB.5 输出高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 端口配置, 推挽输出
	GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	GPIO_SetBits(GPIOD, GPIO_Pin_2); 						 //PD.2 输出高 
}

/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */

u32	Serial_KeyPressed(u8 *key)
{
	if ((USART1->DR == 0x32)||(USART1->DR == 0x33) || (USART1->DR == 0x34))
	{
		*key = (uint8_t) USART1->DR;
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
  * @brief  Get a key from the HyperTerminal
  * @param  None
  * @retval The Key Pressed
  */

u8 Get_Key(void)
{
	u8 Key = 0;
	while (1)
	{
		if (Serial_KeyPressed((u8 *)&Key))
			break;
	}
	return Key;
}
