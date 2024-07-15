#include "usr_system.h"
#include <string.h>
#include <stdlib.h>
#include "usr_lin.h"
#include "main.h"

#define LIN_BUFFER_LEN 	8
#define LIN_TEMP_ID 		47
#define LIN_TEMP_WR_ID 	7

#define LIN_HEADER_LEN 4
#define LIN_SYNC_BREAK_1 0x00
#define LIN_SYNC_BREAK_2 0x80
#define LIN_SYNC_FIELD 0x55

uint8_t CalculateLINCrc(const uint8_t *f_p, uint8_t f_len);


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

// THIS FUNCTION IS GOING TO BE DELETED
HAL_StatusTypeDef LIN_TxMessage(const uint8_t *data, const uint8_t data_size)
{
	uint8_t f_Txbuffer[LIN_BUFFER_LEN]={0};
	
	f_Txbuffer[0] = LIN_TEMP_ID;
	f_Txbuffer[1] = LIN_TEMP_WR_ID;
	
	memcpy(&f_Txbuffer[2], data, data_size);
	
	f_Txbuffer[7]=CalculateLINCrc(f_Txbuffer, 7);
	
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	
	return HAL_UART_Transmit(&huart1, f_Txbuffer, LIN_BUFFER_LEN, 1000);
}



//void UsrLIN_RxCallback(void)
//{
//	LinRxBuf[LinDataRxLenght++] = LIN_SingleData;

//	if(LinDataRxLenght == 4 && LinRxBuf[0] == 0 && LinRxBuf[1] == 0)
//	{
//		LinDataRxLenght = 0;
//		g_LinHeaderRxCpltFlg = true;
//		g_LinStat = true;
//	}
//	else if (!(LinRxBuf[0] == 0 && LinRxBuf[1] == 0) && LinDataRxLenght >= 2 && (LinDataRxLenght == 8+1))
//	{
//		if(LinRxBuf[LinDataRxLenght-1] == CalculateCrcProc(LinRxBuf, LinDataRxLenght-1))
//		{
//			g_LinResponseRxCpltFlg = true;
//			g_LinStat = true;
//		}
//		else
//		{
//			g_LinResponseRxCpltFlg = false;
//			memset(LinRxBuf, 0, sizeof(LinRxBuf));
//		}
//		
//		LinDataRxLenght = 0;	
//	}
//	else if (LinDataRxLenght >= 13)
//	{
//		LinDataRxLenght = 0;
//	}
//	
//	LinBusCheckTimer = 0;
//	HAL_HalfDuplex_EnableReceiver(&huart1);
//	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);	
//}