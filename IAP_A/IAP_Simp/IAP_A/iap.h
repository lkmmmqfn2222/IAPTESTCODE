#ifndef _IAP_H
#define _IAP_H
#include "bsp.h"

typedef  void(*IAPFun)(void);//����һ���������͵Ĳ���.
#define FLASH_APP1_ADDR		0x08008000

void iap_load_app(u32 appxaddr);
void IAPWriteAppbin(u32 appxaddr, u8 *appbuf, u32 appsize);
void Jump_To_APP(uint32_t appxaddr);

//void Serial_Download(void);
//void Serial_Upload(void);
//void Refresh_Thread(void);

#endif