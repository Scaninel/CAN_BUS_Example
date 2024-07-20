#include "usr_system.h"
#include <string.h>
#include <stdlib.h>
#include "usr_lin.h"
#include "usr_can.h"
#include "main.h"

#define LIN_BUFFER_LEN 	8
#define LIN_HEADER_LEN 4
#define LIN_SYNC_BREAK_1 0x00
#define LIN_SYNC_BREAK_2 0x80
#define LIN_SYNC_FIELD 0x55

#define LIN_TEMP_WR_ID 	7
#define LIN_TEMP_R_ID 	77
#define LIN_CAN_ST_ID 	55

uint8_t CalculateLINCrc(const uint8_t *f_p, uint8_t f_len);


void LINProc(void)
{
	if(g_LIN_TempTx)
	{
		g_LIN_TempTx = false;
		PRO_LIN_TxHeaderData(LIN_TEMP_WR_ID, &g_receivedTemp, 1);
	}
	
	if(g_LIN_CanStTx)
	{
		g_LIN_CanStTx = false;
		PRO_LIN_TxHeaderData(LIN_CAN_ST_ID, &g_CanSt, 1);
	}
}

HAL_StatusTypeDef PRO_LIN_TxHeaderData(uint8_t id, const uint8_t *data, uint8_t data_length)
{
	HAL_StatusTypeDef f_TxStat = HAL_ERROR;
	
	uint8_t f_header [LIN_HEADER_LEN] = {0};
	
	f_header[0]= LIN_SYNC_BREAK_1;
	f_header[1]= LIN_SYNC_BREAK_2;
	f_header[2]= LIN_SYNC_FIELD;
	f_header[3]= id;
	
	uint8_t *f_TxBuf = malloc(LIN_HEADER_LEN + data_length + 1);
	
	if(f_TxBuf == NULL)
		return HAL_ERROR;
	
	memcpy(f_TxBuf, f_header, LIN_HEADER_LEN);
	memcpy(&f_TxBuf[LIN_HEADER_LEN], data, data_length);
	
	f_TxBuf[LIN_HEADER_LEN + data_length] = CalculateLINCrc(data, data_length);
	
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	
	f_TxStat = HAL_UART_Transmit(&huart1, f_TxBuf, LIN_HEADER_LEN + data_length + 1, 1000);
	
	HAL_HalfDuplex_EnableReceiver(&huart1);
	free(f_TxBuf);
	
	HAL_Delay(500);
	
	return f_TxStat;
}

HAL_StatusTypeDef PRO_LIN_TxHeader(uint8_t id)
{
	HAL_StatusTypeDef f_TxStat = HAL_ERROR;
	
	uint8_t f_header [LIN_HEADER_LEN] = {0};
	
	f_header[0]= 0x00;
	f_header[1]= 0x80;
	f_header[2]= 0x55;
	f_header[3]= id;
	
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	
	f_TxStat = HAL_UART_Transmit(&huart1, f_header, LIN_HEADER_LEN, 1000);
	
	HAL_HalfDuplex_EnableReceiver(&huart1);
	
	HAL_Delay(500);
	
	return f_TxStat;
}
uint8_t CalculateLINCrc(const uint8_t *f_p, uint8_t f_len)
{
    uint8_t crc = 0, len = 0;

    while (len < f_len)
    {
        uint8_t extract = f_p[len++];
        for (uint8_t i = 8; i; i--)
        {
            uint8_t sum = (crc ^ extract) & 0x01;
            crc >>= 1;
					
            if (sum)
            {
                crc ^= 0x8C;
            }
            extract >>= 1;
        }
    }
    return crc;
}

