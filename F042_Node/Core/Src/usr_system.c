#include "usr_system.h"
#include "usr_can.h"
#include "usr_lin.h"

volatile uint32_t systemTimer;
volatile uint8_t g_ADC_ValReady;

uint32_t adcValues[3];
double temperatureVolt;
uint8_t temperatureCe;
HAL_StatusTypeDef CAN_RxStat = HAL_ERROR;

void LedBlink(void);

void UsrSystemInit(void)
{
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);
	
	HAL_TIM_Base_Start_IT(&htim16);
	
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
	HAL_CAN_Start(&hcan);
	
	HAL_ADC_Start_DMA(&hadc, adcValues, 3);
}

void UsrSystemGeneral(void)
{
	if (g_ADC_ValReady)
	{
		g_ADC_ValReady = false;
		
		temperatureVolt = adcValues[0] * 3.3 / 4096;
		temperatureCe = (uint8_t)(temperatureVolt * 100);
		
		CAN_RxStat = UsrCanTxProccess(&temperatureCe, sizeof(temperatureCe));
		if(CAN_RxStat == HAL_OK)
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