#include "usr_system.h"
#include "usr_can.h"
#include "usr_lin.h"

volatile uint32_t systemTimer;
volatile uint8_t g_ADC_ValReady;

uint32_t adcValues[3];
double temperatureVolt;
uint8_t temperatureCe;

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
	//UsrCanTxProccess();
	//UsrLinRxProccess();
	//UsrLinTxProccess();
	
	if (g_ADC_ValReady)
	{
		g_ADC_ValReady = false;
		
		temperatureVolt = adcValues[0] * 3.3 / 4096;
		temperatureCe = (uint8_t)(temperatureVolt * 100);	
	}
}