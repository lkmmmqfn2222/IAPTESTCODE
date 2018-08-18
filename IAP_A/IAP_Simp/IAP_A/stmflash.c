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




/************************************************************************/
/*	  The Flowing Code Comes From ST Official UART IAP Project          */
/************************************************************************/

static uint32_t GetSector(uint32_t Address);

/**
 *@brief Unlock Flash for write access
 *@param None
 *@retval None
 */

void FLASH_If_Init(void)
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR | FLASH_FLAG_PGERR |
		FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR );
}


/*
*@Name Flash_If_Erase
*@brief This function does an erase of all user flash area
*@param StartSector: start of user flash area
*@retval 0: user flash area successfully erase 
*		 1: error occurred
*/

uint32_t Flash_If_Erase(uint32_t StartSector)
{
	uint32_t UserStartSector = FLASH_Sector_0, i = 0;
	UserStartSector = GetSector(StartSector);
	for (i = UserStartSector; i < FLASH_Sector_8; i += 0x800)//2k one page
	{
		if (FLASH_ErasePage(i)!=FLASH_COMPLETE)
		{
			return 1;
		}
	}
	return 0;
}


/*
*@Name GetSector
*@brief Gets the sector of a given address
*@param Address: Flash address
*@retval The sector of a given address
*/

static uint32_t GetSector(uint32_t Address)
{
	uint32_t sector = 0;
	if((Address<FLASH_Sector_1)&&(Address>=FLASH_Sector_0))
	{
		sector = FLASH_Sector_0;
	}
	else if ((Address<FLASH_Sector_2) && (Address >= FLASH_Sector_1))
	{
		sector = FLASH_Sector_1;
	}
	else if ((Address < FLASH_Sector_3) && (Address >= FLASH_Sector_2))
	{
		sector = FLASH_Sector_2;
	}
	else if ((Address < FLASH_Sector_4) && (Address >= FLASH_Sector_3))
	{
		sector = FLASH_Sector_3;
	}
	else if ((Address < FLASH_Sector_5) && (Address >= FLASH_Sector_4))
	{
		sector = FLASH_Sector_4;
	}
	else if ((Address < FLASH_Sector_6) && (Address >= FLASH_Sector_5))
	{
		sector = FLASH_Sector_5;
	}
	else if ((Address < FLASH_Sector_7) && (Address >= FLASH_Sector_6))
	{
		sector = FLASH_Sector_6;
	}
	else
	{
		sector = FLASH_Sector_7;
	}
	return sector;
}

/**
* @brief  This function writes a data buffer in flash (data are 32-bit aligned).
* @note   After writing data buffer, the flash content is checked.
* @param  FlashAddress: start address for writing data buffer
* @param  Data: pointer on data buffer
* @param  DataLength: length of data buffer (unit is 32-bit word)
* @retval 0: Data successfully written to Flash memory
*         1: Error occurred while writing data in Flash memory
*         2: Written Data in flash memory is different from expected one
*/

uint32_t FLASH_If_Write(__IO uint32_t *FlashAddress, uint32_t *Data, uint32_t Datalength)
{
	uint32_t i = 0;
	for (i = 0; (i<Datalength)&&(*FlashAddress<=(USER_FLASH_END_ADDRESS-4));i++)
	{
		if (FLASH_ProgramWord(*FlashAddress,*(uint32_t*)(Data+i))==FLASH_COMPLETE)
		{
			/* Check the written value */
			if (*(uint32_t*)FlashAddress!=*(uint32_t*)Data)
			{
				/*Flash content doesn't match SRAM content*/
				return 2;
			}
			*FlashAddress += 4;
		}
		else
		{
			/* Error occurred while writing data in Flash memory */
			return 1;
		}
	}
	return 0;
}

