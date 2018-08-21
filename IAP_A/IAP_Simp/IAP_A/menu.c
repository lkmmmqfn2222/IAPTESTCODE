#include "menu.h"
#include "myprintf.h"
#include "bsp.h"
#include "iap.h"
#include "delay.h"
#include "ymodem.h"
#include "stmflash.h"
#include "stm32f10x_flash.h"
#include "common.h"

pFunction Jump_To_Application;
uint32_t JumpAddress;

__IO uint32_t FlashProtection = 0;

uint8_t tab_1024[1024] = { 0 };
uint8_t FileName[FILE_NAME_LENGTH];



void SerialDownload(void);
void SerialUpload(void);



/**
* @brief  Download a file via serial port
* @param  None
* @retval None
*/

void SerialDownload(void)
{
	uint8_t Number[10] = "          ";
	int32_t Size = 0;

	SerialPutString("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
	Size = Ymodem_Receive(&tab_1024[0]);
	if (Size > 0)
	{
		SerialPutString("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
		SerialPutString(FileName);
		Int2Str(Number, Size);
		SerialPutString("\n\r Size: ");
		SerialPutString(Number);
		SerialPutString(" Bytes\r\n");
		SerialPutString("-------------------\n");
	}
	else if (Size == -1)
	{
		SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r");
	}
	else if (Size == -2)
	{
		SerialPutString("\n\n\rVerification failed!\n\r");
	}
	else if (Size == -3)
	{
		SerialPutString("\r\n\nAborted by user.\n\r");
	}
	else
	{
		SerialPutString("\n\rFailed to receive the file!\n\r");
	}

}



/**
* @brief  Upload a file via serial port.
* @param  None
* @retval None
*/
void SerialUpload(void)
{
	uint8_t status = 0;

	SerialPutString("\n\n\rSelect Receive File\n\r");

	if (GetKey() == CRC16)
	{
		/* Transmit the flash image through ymodem protocol */
		status = Ymodem_Transmit((uint8_t*)APPLICATION_ADDRESS, (const uint8_t*)"UploadedFlashImage.bin", USER_FLASH_SIZE);

		if (status != 0)
		{
			SerialPutString("\n\rError Occurred while Transmitting File\n\r");
		}
		else
		{
			SerialPutString("\n\rFile uploaded successfully \n\r");
		}
	}
}

/**
* @brief  Display the Main Menu on HyperTerminal
* @param  None
* @retval None
*/
void Main_Menu(void)
{
	uint8_t key = 0;

	SerialPutString("\r\n======================================================================");
	SerialPutString("\r\n=              (C) COPYRIGHT 2011 STMicroelectronics                 =");
	SerialPutString("\r\n=                                                                    =");
	SerialPutString("\r\n=  STM32F4xx In-Application Programming Application  (Version 1.0.0) =");
	SerialPutString("\r\n=                                                                    =");
	SerialPutString("\r\n=                                   By MCD Application Team          =");
	SerialPutString("\r\n======================================================================");
	SerialPutString("\r\n\r\n");

	/* Test if any sector of Flash memory where user application will be loaded is write protected */
	if (FLASH_GetReadOutProtectionStatus() == SET)
	{
		FlashProtection = 1;
	}
	else
	{
		FlashProtection = 0;
	}

	while (1)
	{
		SerialPutString("\r\n================== Main Menu ============================\r\n\n");
		SerialPutString("  Download Image To the STM32F4xx Internal Flash ------- 1\r\n\n");
		SerialPutString("  Upload Image From the STM32F4xx Internal Flash ------- 2\r\n\n");
		SerialPutString("  Execute The New Program ------------------------------ 3\r\n\n");

		if (FlashProtection != 0)
		{
			SerialPutString("  Disable the write protection ------------------------- 4\r\n\n");
		}

		SerialPutString("==========================================================\r\n\n");

		/* Receive key */
		key = GetKey();

		if (key == 0x31)
		{
			/* Download user application in the Flash */
			SerialDownload();
		}
		else if (key == 0x32)
		{
			/* Upload user application from the Flash */
			SerialUpload();
		}
		else if (key == 0x33) /* execute the new program */
		{
			JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
			/* Jump to user application */
			Jump_To_Application = (pFunction)JumpAddress;
			/* Initialize user application's Stack Pointer */
			__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
			Jump_To_Application();
		}
		else if ((key == 0x34) && (FlashProtection == 1))
		{
			/* Disable the write protection */
			switch (FLASH_If_DisableWriteProtection())
			{
			case 1:
			{
				SerialPutString("Write Protection disabled...\r\n");
				FlashProtection = 0;
				break;
			}
			case 0:
			{
				SerialPutString("Error: Flash write unprotection failed...\r\n");
				break;
			}
			default:
			{
			}
			}
		}
		else
		{
			if (FlashProtection == 0)
			{
				SerialPutString("Invalid Number ! ==> The number should be either 1, 2 or 3\r");
			}
			else
			{
				SerialPutString("Invalid Number ! ==> The number should be either 1, 2, 3 or 4\r");
			}
		}
	}
}

