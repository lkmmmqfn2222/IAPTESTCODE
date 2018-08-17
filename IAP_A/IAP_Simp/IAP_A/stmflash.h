#ifndef _STMFLASH_H
#define _STMFLASH_H
#include "stm32f10x_flash.h"
#include "bsp.h"

#define STM32F4XX 0


#define STM32_FLASH_SIZE 256//number of sector
#define STM32_FLASH_BASE 0x08000000 	//start address of flash
#define STM_SECTOR_SIZE	2048//sector size of HD

/*   FlashSize = 2kb * sectors  */
#if STM32F4XX
#define FLASH_Sector_0     ((uint16_t)0x0000) /*!< Sector Number 0 */
#define FLASH_Sector_1     ((uint16_t)0x0008) /*!< Sector Number 1 */
#define FLASH_Sector_2     ((uint16_t)0x0010) /*!< Sector Number 2 */
#define FLASH_Sector_3     ((uint16_t)0x0018) /*!< Sector Number 3 */
#define FLASH_Sector_4     ((uint16_t)0x0020) /*!< Sector Number 4 */
#define FLASH_Sector_5     ((uint16_t)0x0028) /*!< Sector Number 5 */
#define FLASH_Sector_6     ((uint16_t)0x0030) /*!< Sector Number 6 */
#define FLASH_Sector_7     ((uint16_t)0x0038) /*!< Sector Number 7 */
#define FLASH_Sector_8     ((uint16_t)0x0040) /*!< Sector Number 8 */
#define FLASH_Sector_9     ((uint16_t)0x0048) /*!< Sector Number 9 */
#define FLASH_Sector_10    ((uint16_t)0x0050) /*!< Sector Number 10 */
#define FLASH_Sector_11    ((uint16_t)0x0058) /*!< Sector Number 11 */
#define FLASH_Sector_12    ((uint16_t)0x0060) /*!< Sector Number 12 */
#define FLASH_Sector_13    ((uint16_t)0x0068) /*!< Sector Number 13 */
#define FLASH_Sector_14    ((uint16_t)0x0070) /*!< Sector Number 14 */
#define FLASH_Sector_15    ((uint16_t)0x0078) /*!< Sector Number 15 */

#define IS_FLASH_SECTOR(SECTOR) (((SECTOR) == FLASH_Sector_0) || ((SECTOR) == FLASH_Sector_1) ||\
                                 ((SECTOR) == FLASH_Sector_2) || ((SECTOR) == FLASH_Sector_3) ||\
                                 ((SECTOR) == FLASH_Sector_4) || ((SECTOR) == FLASH_Sector_5) ||\
                                 ((SECTOR) == FLASH_Sector_6) || ((SECTOR) == FLASH_Sector_7) ||\
                                 ((SECTOR) == FLASH_Sector_8) || ((SECTOR) == FLASH_Sector_9) ||\
                                 ((SECTOR) == FLASH_Sector_10) || ((SECTOR) == FLASH_Sector_11)||\
								 ((SECTOR) == FLASH_Sector_12) || ((SECTOR) == FLASH_Sector_13)||\
								 ((SECTOR) == FLASH_Sector_14) || ((SECTOR) == FLASH_Sector_15))
	
#endif

u16 STMFLASH_ReadHalfWord(u32 faddr);
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);
void Test_Write(u32 WriteAddr, u16 WriteData);

#endif