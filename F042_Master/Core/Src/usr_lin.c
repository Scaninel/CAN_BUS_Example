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

#define LIN_STAT_CHECK_TIME (!(systemTimer % 10000))

uint8_t CalculateLINCrc(const uint8_t *f_p, uint8_t f_len);

uint8_t LIN_SingleData[4];
uint8_t LinDataRxLenght;
uint8_t LinRxBuf[LIN_BUFFER_LEN];
volatile uint8_t LIN_HeaderReceived;
uint8_t g_LinSt;

void LINProc(void)
{
	UsrLinRxProccess();
	
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
	
//	if(LIN_STAT_CHECK_TIME)
//	{
//		PRO_LIN_TxHeader(LIN_TEMP_R_ID);
//	}
}

HAL_StatusTypeDef PRO_LIN_TxHeaderData(uint8_t id, const uint8_t *data, uint8_t data_length)
{
	HAL_StatusTypeDef f_TxStat = HAL_ERROR;
	
	uint8_t f_header [LIN_HEADER_LEN] = {0};
	
	f_header[0]= LIN_SYNC_BREAK_1;
	f_header[1]= LIN_SYNC_BREAK_2;
	f_header[2]= LIN_SYNC_FIELD;
	f_header[3]= id;
	
	uint8_t f_TxBuf [8]= {0};
	
	memcpy(f_TxBuf, f_header, LIN_HEADER_LEN);
	memcpy(&f_TxBuf[LIN_HEADER_LEN], data, data_length);
	
	f_TxBuf[LIN_HEADER_LEN + data_length] = CalculateLINCrc(data, data_length);
	
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	
	f_TxStat = HAL_UART_Transmit(&huart1, f_TxBuf, LIN_HEADER_LEN + 4, 1000);
	
	HAL_HalfDuplex_EnableReceiver(&huart1);
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_Delay(25);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	
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

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_Delay(25);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	
	HAL_Delay(500);
	
	return f_TxStat;
}

void UsrLinRxProccess(void)
{
	if(LIN_HeaderReceived == true)
	{
		if (LinDataRxLenght >= 4 && (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)))
		{
				LIN_HeaderReceived = false;
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);
			
			uint8_t f_RxCrc = LinRxBuf[5];
			uint8_t f_CalCrc = CalculateLINCrc(&LinRxBuf[4], 1);
			
			if(f_RxCrc != f_CalCrc)
			{
				LinDataRxLenght = 0;
				memset(LinRxBuf, 0, sizeof(LinRxBuf));
				memset(LIN_SingleData,0, sizeof(LIN_SingleData));
				return;
			}
			
			if (LIN_TEMP_R_ID == LinRxBuf[3])
			{
				if(g_receivedTemp != LinRxBuf[4])
					g_LinSt = false;
				else
					g_LinSt = true;
			}
			
//			if (LinRxBuf[3] == LIN_TEMP_WR_ID)
//			{
//				g_Received_Temp = LinRxBuf[4];
//			}
//			else if(LinRxBuf[3] == LIN_CAN_ST_ID)
//			{
//				g_Received_CanSt = LinRxBuf[4];
//			}
		

			LinDataRxLenght = 0;
			memset(LinRxBuf, 0, sizeof(LinRxBuf));
			memset(LIN_SingleData,0, sizeof(LIN_SingleData));
		}
	}
	else if (LIN_HeaderReceived == false && LinDataRxLenght && (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		LinDataRxLenght = 0;
		memset(LinRxBuf, 0, sizeof(LinRxBuf));
		memset(LIN_SingleData,0, sizeof(LIN_SingleData));
	}
}

void UsrLIN_RxCallback(void)
{
	if(LIN_HeaderReceived == false)
	{
		LIN_HeaderReceived = true;
		memcpy(&LinRxBuf[0], LIN_SingleData, 4);
		
		if((LinRxBuf[0] != LIN_SYNC_BREAK_1) || (LinRxBuf[1] != LIN_SYNC_BREAK_2) || (LinRxBuf[2] != LIN_SYNC_FIELD))
		{
			LinDataRxLenght=0;
			memset(LinRxBuf, 0, sizeof(LinRxBuf));
			memset(LIN_SingleData,0, sizeof(LIN_SingleData));
			LIN_HeaderReceived = false;
		}
	}
	else
	{
		memcpy(&LinRxBuf[4], LIN_SingleData, 4);
	}
	
	LinDataRxLenght += 4;
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UART_Receive_IT(&huart1, LIN_SingleData, 4);
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
