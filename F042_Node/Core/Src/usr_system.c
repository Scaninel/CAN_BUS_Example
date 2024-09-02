#include "usr_system.h"
#include "usr_can.h"
#include "usr_lin.h"

volatile uint32_t systemTimer;
volatile uint8_t g_ADC_ValReady;

uint32_t adcValues[4];
double temperatureVolt;
uint8_t temperatureCe;
HAL_StatusTypeDef CAN_RxStat = HAL_ERROR;

uint8_t DutyCycle;
uint8_t DutyCycleSent;
uint8_t motorEnbl;
uint8_t motorEnblSent;

void LedBlink(void);

void UsrSystemInit(void)
{
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);
	
	HAL_TIM_Base_Start_IT(&htim16);
	
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
	HAL_CAN_Start(&hcan);
	
	HAL_ADC_Start_DMA(&hadc, adcValues, 4);
}

void UsrSystemGeneral(void)
{
	if (g_ADC_ValReady)
	{
		g_ADC_ValReady = false;
		
		DutyCycle = (uint8_t)((adcValues[0] * 3.3 / 4096) * 100 /3.3);
		
		if((DutyCycle != DutyCycleSent) || (motorEnbl != motorEnblSent))
		{
			uint8_t motorStatBuf[2]={DutyCycle, motorEnbl};
			if(!UsrCanRefSpeedTxProccess(motorStatBuf, 2))
			{
				DutyCycleSent = DutyCycle;
				motorEnblSent = motorEnbl;
				LedBlink();
			}
		}
		
		temperatureVolt = adcValues[1] * 3.3 / 4096;
		temperatureCe = (uint8_t)(temperatureVolt * 100);
		
		if(!UsrCanTempTxProccess(&temperatureCe, sizeof(temperatureCe)))
		{
			LedBlink();
		}
	}
	

	
	
	
	//UsrLinRxProccess();
	//UsrLinTxProccess();
	
	// if (g_ADC_ValReady)
	// {
	// 	g_ADC_ValReady = false;
		
	// 	temperatureVolt = adcValues[0] * 3.3 / 4096;
	// 	temperatureCe = (uint8_t)(temperatureVolt * 100);	
	// }
}


void LedBlink(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_Delay(25);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_Delay(25);
}