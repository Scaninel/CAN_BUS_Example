#include "main.h"
#include "stm32l0xx_hal_uart.h"
#include <string.h>
#include "usr_lin.h"
#include "usr_screen.h"

void UsrLinRxProccess(void);
void ClearLinBuf(void);
HAL_StatusTypeDef PRO_LIN_TxData(uint8_t id, const uint8_t *data, uint8_t data_length);
uint8_t CalculateLINCrc(const uint8_t *f_p, uint8_t f_len);

uint8_t LinDataRxLenght;
uint8_t LinRxBuf[LIN_BUFFER_LEN];
volatile uint8_t g_LIN_MsgReceived;

uint8_t g_LIN_TempRxFlg;
uint8_t g_Received_Temp;
uint8_t g_Received_CanSt = NTW_INIT;
uint8_t g_Received_LinSt = NTW_INIT;

float g_Received_rpm;

uint8_t g_LinSingleMsg;
uint8_t LinHeaderReceived;
volatile uint8_t g_LinIdle;

typedef union U_CONVERTER_TAG
{
    uint8_t buf[4];
    uint16_t u16val;
    uint32_t uival;
    int32_t i32val;
    uint32_t ui32val;
    int32_t ival;
    float fval;
}U_CONVERTER;

U_CONVERTER uConvert;

void UsrLinRxProccess(void)
{
	if (g_LinIdle == true && LinHeaderReceived == true)
	{
		if(LinDataRxLenght == LIN_HEADER_LEN)
		{
			// Header Rx Process
			switch (LinRxBuf[3])
			{
				case LIN_TEMP_R_ID:
					PRO_LIN_TxData(LIN_TEMP_R_ID, &writtenTemp, 1);
					break;

				default:
						break;
			}

			ClearLinBuf();
		}
		else if (LinDataRxLenght > LIN_HEADER_LEN && LinDataRxLenght <= LIN_BUFFER_LEN)
		{
			// Header + Data Rx Process
			switch (LinRxBuf[3])
			{
				case LIN_TEMP_WR_ID:
					g_LIN_TempRxFlg = true;
					g_Received_Temp = LinRxBuf[4];
					break;

				case LIN_NTW_ST_ID:
					g_Received_CanSt = (LinRxBuf[4] & NTW_CAN_BIT) != 0;
					g_Received_LinSt = (LinRxBuf[4] & NTW_LIN_BIT) != 0;
					break;

				case LIN_RPM_WR:
					memcpy(uConvert.buf, &LinRxBuf[4], 4);
					g_Received_rpm = uConvert.fval;
					break;

				default:
						break;
			}

			ClearLinBuf();
		}
		else
		{
			//Unexpected Frame Process
			ClearLinBuf();
		}
	}
}

void ClearLinBuf(void)
{
	LinHeaderReceived = false;
	LinDataRxLenght = false;
	memset(LinRxBuf,0,sizeof(LinRxBuf));
}

void UsrLinRxCallback(void)
{
	g_LinIdle = false;
	LinRxBuf[LinDataRxLenght++] = g_LinSingleMsg;

	if(LinDataRxLenght == LIN_HEADER_LEN && LinRxBuf[0]== LIN_SYNC_BREAK_1 && LinRxBuf[1]== LIN_SYNC_BREAK_2 && LinRxBuf[2]== LIN_SYNC_FIELD)
		LinHeaderReceived = true;

	HAL_UART_Receive_IT(&huart1, &g_LinSingleMsg, 1);
}

HAL_StatusTypeDef PRO_LIN_TxData(uint8_t id, const uint8_t *data, uint8_t data_length)
{
	HAL_StatusTypeDef f_TxStat = HAL_ERROR;

	uint8_t f_TxBuf [LIN_BUFFER_LEN - LIN_HEADER_LEN] = {0};

	memcpy(f_TxBuf, data, data_length);

	f_TxBuf[data_length] = CalculateLINCrc(data, data_length);

	HAL_HalfDuplex_EnableTransmitter(&huart1);

	f_TxStat = HAL_UART_Transmit(&huart1, f_TxBuf, data_length + 1, 1000);

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
					crc ^= 0x8C;

			extract >>= 1;
		}
	}

	return crc;
}
