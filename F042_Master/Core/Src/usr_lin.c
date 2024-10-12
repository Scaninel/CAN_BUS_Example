#include "usr_system.h"
#include <string.h>
#include <stdlib.h>
#include "usr_lin.h"
#include "usr_can.h"
#include "main.h"


#define LIN_HEADER_LEN 4
#define LIN_SYNC_BREAK_1 0x00
#define LIN_SYNC_BREAK_2 0x80
#define LIN_SYNC_FIELD 0x55

#define LIN_TEMP_WR_ID 	7
#define LIN_TEMP_R_ID 	77
#define LIN_NTW_ST_ID 	55
#define LIN_RPM_WR 			99
#define LIN_MST_WR 			88

#define LIN_STAT_CHECK_TIME (!(systemTimer % 10000))
#define LIN_RPM_TX_TIME (!(systemTimer % 500) && g_motorEnbl)

typedef union U_CONVERTER_TAG
{
    uint8_t buf[4];
    uint16_t ui16val;
    int16_t i16val;
    uint32_t ui32val;
    int32_t i32val;
    float fval;
} U_CONVERTER;

void ClearLinBuf(void);
uint8_t CalculateLINCrc(const uint8_t *f_p, uint8_t f_len);

uint8_t LinDataRxLenght;
uint8_t LinRxBuf[LIN_BUFFER_LEN];
volatile uint8_t g_LIN_MsgReceived;
uint8_t g_LinSt;

uint8_t g_LinSingleMsg;
volatile uint8_t g_LinIdle;
uint8_t g_LinHeaderSent;

void LINProc(void)
{
	UsrLinRxProccess();
	
	if (!g_LinHeaderSent)
	{
		if(g_LIN_TempTx)
		{
			g_LIN_TempTx = false;
			PRO_LIN_TxHeaderData(LIN_TEMP_WR_ID, &g_receivedTemp, 1);
		}
		
		if(g_NetworkStTx)
		{
			g_NetworkStTx = false;
			PRO_LIN_TxHeaderData(LIN_NTW_ST_ID, &g_NetworkSt, 1);
		}
		
		if(LIN_RPM_TX_TIME)
		{
			U_CONVERTER uConvert;
			
			uConvert.fval= g_rpmMeasured;
			
			PRO_LIN_TxHeaderData(LIN_RPM_WR, uConvert.buf, 4);
		}
		
		if(g_MotorStTx)
		{
			g_MotorStTx = false;
			uint8_t buf[8] = {0};
			
			buf[0] = g_motorEnbl;
			memcpy(&buf[1], &g_rpmRequest, 2);
			
			PRO_LIN_TxHeaderData(LIN_MST_WR, buf, 3);
		}
	}

	if(LIN_STAT_CHECK_TIME)
	{
		PRO_LIN_TxHeader(LIN_TEMP_R_ID);
		g_LinHeaderSent = true;
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
	
	return f_TxStat;
}

HAL_StatusTypeDef PRO_LIN_TxHeader(uint8_t id)
{
	HAL_StatusTypeDef f_TxStat = HAL_ERROR;
	
	uint8_t f_header [LIN_HEADER_LEN] = {0};
	
	f_header[0]= LIN_SYNC_BREAK_1;
	f_header[1]= LIN_SYNC_BREAK_2;
	f_header[2]= LIN_SYNC_FIELD;
	f_header[3]= id;
	
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	
	f_TxStat = HAL_UART_Transmit(&huart1, f_header, LIN_HEADER_LEN, 1000);
	
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UART_Receive_IT(&huart1, &g_LinSingleMsg, 1);

//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
//	HAL_Delay(25);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	
	return f_TxStat;
}

void UsrLinRxProccess(void)
{
	if (g_LinIdle == true && g_LinHeaderSent == true && LinDataRxLenght)
	{
		g_LinHeaderSent = false;
		
		uint8_t f_RxCrc = LinRxBuf[LinDataRxLenght - 1];
		uint8_t f_CalCrc = CalculateLINCrc(LinRxBuf, LinDataRxLenght - 1);
		
		if(f_RxCrc != f_CalCrc)
		{
			goto exit_func;
		}

		if(g_receivedTemp != LinRxBuf[0])
			g_NetworkSt &= ~NTW_LIN_BIT;
		else
			g_NetworkSt |= NTW_LIN_BIT;

		exit_func:
		ClearLinBuf();
	}
}

void ClearLinBuf(void)
{
	LinDataRxLenght = 0;
	memset(LinRxBuf,0,sizeof(LinRxBuf));
}

void UsrLinRxCallback(void)
{
	g_LinIdle = false;
	LinRxBuf[LinDataRxLenght++] = g_LinSingleMsg;
	
	HAL_UART_Receive_IT(&huart1, &g_LinSingleMsg, 1);
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
