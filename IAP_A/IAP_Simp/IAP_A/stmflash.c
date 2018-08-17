#include "stmflash.h"
#include "stm32f10x_flash.h"

/***************************************************
**
**
**faddr:the address of halfword be read
**
**
**
****************************************************/

u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr;
}
/***************************************************
**
**
**WriteAddr:start address
**pBuffer:the pr of the data
**NumToWrite:the number of half word
**
****************************************************/

void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
	u16 i;
	for (i = 0; i < NumToWrite; i++)
	{
		FLASH_ProgramHalfWord(WriteAddr, pBuffer[i]);
		WriteAddr += 2;//addr +=2.
	}
}

/***************************************************
**
**
**WriteAddr:start address
**pBuffer:the pr of the data
**NumToWrite:the number of half word
**
****************************************************/

u16 STMFLASH_BUF[STM_SECTOR_SIZE / 2];//
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
	u32 secpos;	   //address of sector
	u16 secoff;	   //address offset of specify sector
	u16 secremain; //the remainning address of the sector   
	u16 i;
	u32 offaddr;   //offset about 0X08000000
	if (WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))return;//∑«∑®µÿ÷∑
	FLASH_Unlock();						
	offaddr = WriteAddr - STM32_FLASH_BASE;		
	secpos = offaddr / STM_SECTOR_SIZE;			// 0~127 for STM32F103RBT6
	secoff = (offaddr%STM_SECTOR_SIZE) / 2;		//2byte is the base
	secremain = STM_SECTOR_SIZE / 2 - secoff;		  
	if (NumToWrite <= secremain)secremain = NumToWrite;
	while (1)
	{
		STMFLASH_Read(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);//
		for (i = 0; i < secremain; i++)//verify the data 
		{
			if (STMFLASH_BUF[secoff + i] != 0XFFFF)
				break;//need to earse	  
		}
		if (i < secremain)//
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE);//earse the sector
			for (i = 0; i<secremain; i++)//copy data
			{
				STMFLASH_BUF[i + secoff] = pBuffer[i];
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2); 
		}
		else STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain); 				   
		if (NumToWrite == secremain)
			break;//finish writing
		else
		{
			secpos++;				//sector address +1
			secoff = 0;				//set ossset 0 	 
			pBuffer += secremain;  	//
			WriteAddr += secremain;	//   
			NumToWrite -= secremain;	//(16bit)--
			if (NumToWrite>(STM_SECTOR_SIZE / 2))secremain = STM_SECTOR_SIZE / 2;//the next sector is always not finish
			else secremain = NumToWrite;//finish writing at the next sector
		}
	};
	FLASH_Lock();
}

void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)
{
	u16 i;
	for (i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);//read 2 byte
		ReadAddr += 2;//offset 2 byte	
	}
}

void Test_Write(u32 WriteAddr, u16 WriteData)
{
	STMFLASH_Write(WriteAddr, &WriteData, 1);//write a word 
}


/*****************************
 **
 **
 **
 *******************************/