#include "usr_system.h"
#include "usr_lin.h"

#define LIN_TX_PERIOD 5000
#define LIN_BUS_ERROR_TIME 100

volatile uint8_t g_ButtonPressed;

uint8_t LIN_SingleData;
volatile uint8_t g_LinHeaderRxCpltFlg;
volatile uint8_t g_LinResponseRxCpltFlg;
uint8_t g_LinHeaderRxId;
volatile uint32_t LinBusCheckTimer;
volatile uint32_t LinTxTimer;
uint8_t LinDataRxLenght;

uint8_t LinRxBuf[13];

uint8_t CalculateCrcProc(const uint8_t *f_p, uint8_t f_len);

void UsrLinTxProccess(void)
{
	if (LinTxTimer > LIN_TX_PERIOD)
	{
		LinTxTimer = 0;

		uint8_t Txdata[8]={0};
		
		Txdata[0]=20;
		Txdata[1]=47;
		
		Txdata[5]=0xCA;

		if (UsrLIN_HeaderTx(0xCA) == HAL_OK)
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
			HAL_Delay(100);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
			HAL_Delay(1000);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
			HAL_Delay(1000);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
			HAL_Delay(1000);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
			HAL_Delay(1000);
		}
	}
}

void UsrLinRxProccess(void)
{
	if(g_LinHeaderRxCpltFlg)
	{
		g_LinHeaderRxCpltFlg = false;
		g_LinHeaderRxId = LinRxBuf[3];
		memset(LinRxBuf, 0, sizeof(LinRxBuf));
		
		UsrLedBlink(100,1);
		
		uint8_t f_respTxBuf[8] = {0};
		
		for(int i=0;i<8;i++)
		{
			f_respTxBuf[i]= i*11;
		}
		
		if(UsrLIN_ResponseTx(f_respTxBuf, 8) == HAL_OK)
		{
			UsrLedBlink(250,3);
		}
	}
	else if (LinDataRxLenght && !g_LinHeaderRxCpltFlg && LinBusCheckTimer > LIN_BUS_ERROR_TIME)
	{
		LinDataRxLenght = 0;
		memset(LinRxBuf, 0, sizeof(LinRxBuf));
	}
}

HAL_StatusTypeDef UsrLIN_HeaderTx(const uint8_t id)
{
	uint8_t f_Txbuffer[4]={0,0,0x55, id};
	
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	
	return HAL_UART_Transmit(&huart1, f_Txbuffer, 4, 1000);
}

HAL_StatusTypeDef UsrLIN_ResponseTx(const uint8_t *data, size_t dataSize)
{
	if (dataSize > 8)
		return HAL_ERROR;
	
	uint8_t f_Txbuffer[9]={0};

	memcpy(f_Txbuffer, data, dataSize);
	f_Txbuffer[dataSize+1] = CalculateCrcProc(data, dataSize);
	
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	
	return HAL_UART_Transmit(&huart1, f_Txbuffer, dataSize+1, 1000);
}

void UsrLIN_RxCallback(void)
{
	LinRxBuf[LinDataRxLenght++] = LIN_SingleData;

	if(LinDataRxLenght == 4 && LinRxBuf[0] == 0 && LinRxBuf[1] == 0)
	{
		LinDataRxLenght = 0;
		g_LinHeaderRxCpltFlg = true;
	}
	else if (!(LinRxBuf[0] == 0 && LinRxBuf[1] == 0) && LinDataRxLenght >= 2 && (LinDataRxLenght == 8+1))
	{
		if(LinRxBuf[LinDataRxLenght-1] == CalculateCrcProc(LinRxBuf, LinDataRxLenght-1))
			g_LinResponseRxCpltFlg = true;
		else
		{
			g_LinResponseRxCpltFlg = false;
			memset(LinRxBuf, 0, sizeof(LinRxBuf));
		}
		
		LinDataRxLenght = 0;	
	}
	else if (LinDataRxLenght >= 13)
	{
		LinDataRxLenght = 0;
	}
	
	LinBusCheckTimer = 0;
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);	
}

void UsrLedBlink(const uint32_t timeout, const uint8_t blinkCount)
{
	for(int i=0;i<blinkCount;i++)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
		UsrDelay(timeout);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
		UsrDelay(timeout);
	}
}

void UsrDelay(const uint32_t timeout)
{
	systemTimer = 0;
	while (systemTimer < timeout)
		;
}

uint8_t CalculateCrcProc(const uint8_t *f_p, uint8_t f_len)
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