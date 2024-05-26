#include "usr_system.h"
#include <string.h>
#include "usr_lin.h"
#include "main.h"

#define LIN_BUFFER_LEN 	8
#define LIN_TEMP_ID 		47
#define LIN_TEMP_WR_ID 	7

uint8_t CalculateLINCrc(const uint8_t *f_p, uint8_t f_len);

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