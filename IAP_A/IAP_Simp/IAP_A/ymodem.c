#include "ymodem.h"
#include "stmflash.h"
#include "string.h"
#include "common.h"


/**
* @brief  Test to see if a key has been pressed on the HyperTerminal
* @param  key: The key pressed
* @retval 1: Correct
*         0: Error
*/
uint32_t SerialKeyPressed(uint8_t *key)
{

	if (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) != RESET)
	{
		*key = (uint8_t)EVAL_COM1->DR;
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
* @brief  Receive byte from sender
* @param  c: Character
* @param  timeout: Timeout
* @retval 0: Byte received
*        -1: Timeout
*/
static  int32_t Receive_Byte(uint8_t *c, uint32_t timeout)
{
	while (timeout-- > 0)
	{
		if (SerialKeyPressed(c) == 1)
		{
			return 0;
		}
	}
	return -1;
}

/**
* @brief  Send a byte
* @param  c: Character
* @retval 0: Byte sent
*/
static uint32_t Send_Byte(uint8_t c)
{
	SerialPutChar(c);
	return 0;
}

/**
* @brief  Receive a packet from sender
* @param  data
* @param  length
* @param  timeout
*     0: end of transmission
*    -1: abort by sender
*    >0: packet length
* @retval 0: normally return
*        -1: timeout or packet error
*         1: abort by user
*/
static int32_t Receive_Packet(uint8_t *data, int32_t *length, uint32_t timeout)
{
	uint16_t i, packet_size;
	uint8_t c;
	*length = 0;
	if (Receive_Byte(&c, timeout) != 0)
	{
		return -1;
	}
	switch (c)
	{
	case SOH:
		packet_size = PACKET_SIZE;
		break;
	case STX:
		packet_size = PACKET_1K_SIZE;
		break;
	case EOT:
		return 0;
	case CA:
		if ((Receive_Byte(&c, timeout) == 0) && (c == CA))
		{
			*length = -1;
			return 0;
		}
		else
		{
			return -1;
		}
	case ABORT1:
	case ABORT2:
		return 1;
	default:
		return -1;
	}
	*data = c;
	for (i = 1; i < (packet_size + PACKET_OVERHEAD); i++)
	{
		if (Receive_Byte(data + i, timeout) != 0)
		{
			return -1;
		}
	}
	if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
	{
		return -1;
	}
	*length = packet_size;
	return 0;
}