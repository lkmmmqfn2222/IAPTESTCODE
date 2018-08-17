#ifndef _BSP_H
#define _BSP_H
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#define  u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define USART_REC_LEN 10*1024 
extern u8  USART_RX_BUF[USART_REC_LEN];
extern u16 USART_RX_STA;
extern u16 USART_RX_CNT;

#define KEY0 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
#define KEY1 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //output
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //
#define LED0 PAout(8)// PB5
#define LED1 PDout(2)// PE5	


volatile u16 Applength;

void Key_Init(void);
u8 Key_Scan(u8 mode);
void UART_Init(u32 bound);
void LED_Init(void);
void SYS_Dinit(void);

u32	Serial_KeyPressed(u8 *key);
u8 Get_Key(void);

#endif // !_BSP_H
