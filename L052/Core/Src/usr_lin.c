#include "usr_system.h"
#include "usr_lin.h"
#include <stdlib.h>
#include "stm32l0xx_hal_uart.h"
#include "usr_screen.h"

#define NB_OF_GENERATED_RANDOM_NUMBERS 2

#define LIN_HEADER_LEN 4
#define LIN_SYNC_BREAK_1 0x00
#define LIN_SYNC_BREAK_2 0x80
#define LIN_SYNC_FIELD 0x55

#define LIN_TEMP_WR_ID 	7
#define LIN_TEMP_R_ID 	77
#define LIN_NTW_ST_ID 	55

void ClearLinBuf(void);
uint8_t RandomNumbersGeneration(uint32_t *aRandom32bit);
uint8_t CalculateLINCrc(const uint8_t *f_p, uint8_t f_len);
HAL_StatusTypeDef PRO_LIN_TxHeaderData(uint8_t id, const uint8_t *data, uint8_t data_length);
HAL_StatusTypeDef PRO_LIN_TxData(uint8_t id, const uint8_t *data, uint8_t data_length);
HAL_StatusTypeDef PRO_LIN_TxHeader(uint8_t id);

uint8_t LinDataRxLenght;
uint8_t LinRxBuf[LIN_BUFFER_LEN];
volatile uint8_t g_LIN_MsgReceived;

uint8_t g_LIN_TempRxFlg;
uint8_t g_Received_Temp;
uint8_t g_Received_CanSt = NTW_INIT;
uint8_t g_Received_LinSt = NTW_INIT;

uint8_t g_LinSingleMsg;
uint8_t LinHeaderReceived;
volatile uint8_t g_LinIdle;

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
	
	return f_TxStat;
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
					crc ^= 0x8C;

			extract >>= 1;
		}
	}

	return crc;
}

//HAL_StatusTypeDef PRO_LIN_TxHeaderData(uint8_t id, const uint8_t *data, uint8_t data_length)
//{
//	HAL_StatusTypeDef f_TxStat = HAL_ERROR;
//	
//	uint8_t f_header [LIN_HEADER_LEN] = {0};
//	
//	f_header[0]= LIN_SYNC_BREAK_1;
//	f_header[1]= LIN_SYNC_BREAK_2;
//	f_header[2]= LIN_SYNC_FIELD;
//	f_header[3]= id;
//	
//	uint8_t *f_TxBuf = malloc(LIN_HEADER_LEN + data_length + 1);
//	
//	if(f_TxBuf == NULL)
//		return HAL_ERROR;
//	
//	memcpy(f_TxBuf, f_header, LIN_HEADER_LEN);
//	memcpy(&f_TxBuf[LIN_HEADER_LEN], data, data_length);
//	
//	f_TxBuf[LIN_HEADER_LEN + data_length] = CalculateLINCrc(data, data_length);
//	
//	HAL_HalfDuplex_EnableTransmitter(&huart1);
//	
//	f_TxStat = HAL_UART_Transmit(&huart1, f_TxBuf, LIN_HEADER_LEN + data_length + 1, 1000);
//	
//	HAL_HalfDuplex_EnableReceiver(&huart1);
//	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);
//	free(f_TxBuf);
//	
//	return f_TxStat;
//}

//HAL_StatusTypeDef PRO_LIN_TxHeader(uint8_t id)
//{
//	HAL_StatusTypeDef f_TxStat = HAL_ERROR;
//	
//	uint8_t f_header [LIN_HEADER_LEN] = {0};
//	
//	f_header[0]= 0x00;
//	f_header[1]= 0x80;
//	f_header[2]= 0x55;
//	f_header[3]= id;
//	
//	HAL_HalfDuplex_EnableTransmitter(&huart1);
//	
//	f_TxStat = HAL_UART_Transmit(&huart1, f_header, LIN_HEADER_LEN, 1000);
//	
//	HAL_HalfDuplex_EnableReceiver(&huart1);
//	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);
//	
//	return f_TxStat;
//}




















uint8_t RandomNumbersGeneration(uint32_t *aRandom32bit)
{
  register uint8_t index = 0;

  /* Initialize random numbers generation */

  /* Generate Random 32bit Numbers */
  for(index = 0; index < NB_OF_GENERATED_RANDOM_NUMBERS; index++)
  {
#if (USE_TIMEOUT == 1)
    Timeout = RNG_GENERATION_TIMEOUT;
#endif /* USE_TIMEOUT */

    /* Wait for DRDY flag to be raised */
    while (!LL_RNG_IsActiveFlag_DRDY(RNG))
    {
#if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag()) 
      { 
        if(Timeout-- == 0)
        {
          /* Time-out occurred. Set LED to blinking mode */
          LED_Blinking(LED_BLINK_SLOW);
        }
      } 
#endif /* USE_TIMEOUT */
    }

    /* Check if error occurs */
    if (   (LL_RNG_IsActiveFlag_CECS(RNG) )
        || (LL_RNG_IsActiveFlag_SECS(RNG))  )
    {
      /* Clock or Seed Error detected. Set LED to blinking mode (Error type)*/
      return false;
    }
    
    /* Otherwise, no error detected : Value of generated random number could be retrieved
       and stored in dedicated array */
    aRandom32bit[index] = LL_RNG_ReadRandData32(RNG);
  }

  /* Stop random numbers generation */
  LL_RNG_Disable(RNG);
	
	return true;
}



void UsrLedBlink(const uint32_t timeout, const uint8_t blinkCount)
{
	for(int i=0;i<blinkCount;i++)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
		UsrDelay(timeout);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
		UsrDelay(timeout);
	}
}

void UsrDelay(const uint32_t timeout)
{
	systemTimer = 0;
	while (systemTimer < timeout)
		;
}

