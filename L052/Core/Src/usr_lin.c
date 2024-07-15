#include "usr_system.h"
#include "usr_lin.h"

#define DEVICE_LIN_ID 0xCC
#define LIN_TX_PERIOD 5000
#define LIN_BUS_ERROR_TIME 100
#define TEMPERATURE_OFFSET 26
#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
#define VREFINT_CAL_ADDR ((uint8_t*) ((uint32_t) 0x1FFFF7BA))
#define NB_OF_GENERATED_RANDOM_NUMBERS 2

#define LIN_SYNC_BREAK_1 0x00
#define LIN_SYNC_BREAK_2 0x80
#define LIN_SYNC_FIELD 0x55



volatile uint8_t g_ButtonPressed;

uint8_t LIN_SingleData;
volatile uint8_t g_LinHeaderRxCpltFlg;
volatile uint8_t g_LinResponseRxCpltFlg;
uint8_t g_LinHeaderRxId;
volatile uint32_t LinBusCheckTimer;
volatile uint32_t LinTxTimer;
uint8_t LinDataRxLenght;

uint8_t LinRxBuf[LIN_BUFFER_LEN];

uint8_t RandomNumbersGeneration(uint32_t *aRandom32bit);
uint8_t CalculateLINCrc(const uint8_t *f_p, uint8_t f_len);
uint32_t Getui32Proc(const uint8_t *f_ptr, uint8_t *f_pIndis);
void Insertui32Proc(uint32_t f_val, uint8_t *f_ptr, uint8_t *f_pIndis);

uint8_t VREFINT_CAL_ADDR_VAL;
double f_refVolt, f_tempVolt;
uint32_t f_tempAdcVal;
uint32_t f_vrefAdcVal;


volatile uint8_t LIN_HeaderReceived;
uint8_t Received_Temp;

volatile uint32_t g_LIN_Timeout;
volatile uint8_t LIN_DataReceived;


void UsrLinRxProccess(void)
{
if(LIN_HeaderReceived == true)
{
	if (LinDataRxLenght > 4 && g_LIN_Timeout > 75)
	{
			LIN_HeaderReceived = false;
						
			if (LIN_TEMP_WR_ID == LinRxBuf[3])
			{
				uint8_t f_RxCrc = LinRxBuf[5];
				
				uint8_t f_CalCrc = CalculateLINCrc(&LinRxBuf[4], 1);
				
				if(f_RxCrc == f_CalCrc)
					Received_Temp = LinRxBuf[4];
			}

			LinDataRxLenght = 0;
			memset(LinRxBuf, 0, sizeof(LinRxBuf));
	}
}
else if (LIN_HeaderReceived == false && LinDataRxLenght && g_LIN_Timeout > 150)
{
	LinDataRxLenght = 0;
	memset(LinRxBuf, 0, sizeof(LinRxBuf));
}
		
	
	
	
	
//	if(LIN_HeaderReceived)
//	{
//		LIN_HeaderReceived = false;
//		
//		uint8_t f_calCrc = CalculateLINCrc(LinRxBuf,7);
//		
//		if (LinRxBuf[7] == f_calCrc)
//		{
//			if (LIN_TEMP_WR_ID == LinRxBuf[1])
//			{
//				Received_Temp = LinRxBuf[2];
//			}
//		}
//		
//		HAL_HalfDuplex_EnableReceiver(&huart1);
//		memset(LinRxBuf, 0, sizeof(LinRxBuf));
//	}
}

void UsrLIN_RxCallback(void)
{
	LinRxBuf[LinDataRxLenght++] = LIN_SingleData;

	if(LinDataRxLenght == 4)
	{
		if(LIN_SYNC_BREAK_1 == LinRxBuf[0] && LIN_SYNC_BREAK_2 == LinRxBuf[1] && LIN_SYNC_FIELD == LinRxBuf[2])	
		{
			LIN_HeaderReceived = true;
		}
		else
		{
			LinDataRxLenght = 0;
			memset(LinRxBuf, 0, sizeof(LinRxBuf));
			LIN_HeaderReceived = false;
		}
	}

	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);	
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






//---------------------------------------------------------------------------------------------------------------//
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
	f_Txbuffer[dataSize] = CalculateLINCrc (data, dataSize);
	HAL_HalfDuplex_EnableTransmitter(&huart1);
	
	return HAL_UART_Transmit(&huart1, f_Txbuffer, dataSize+1, 1000);
}

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

uint32_t Getui32Proc(const uint8_t *f_ptr, uint8_t *f_pIndis)
{
    uint8_t indis = *f_pIndis;

    U_CONVERTER uConvert;
    for (int8_t i = 3; i >= 0; i--)
    {
        uConvert.buf[i] = f_ptr[indis++];
    }

    *f_pIndis = indis;
    return uConvert.uival;
}

void Insertui32Proc(uint32_t f_val, uint8_t *f_ptr, uint8_t *f_pIndis)
{
    uint8_t indis = *f_pIndis;

    U_CONVERTER uConvert;
    uConvert.ui32val = f_val;

    for (int8_t i = 0; i < 4; i++)
        f_ptr[indis++] = uConvert.buf[3 - i];

    *f_pIndis = indis;
}