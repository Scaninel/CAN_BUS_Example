#include "usr_system.h"
#include "usr_can.h"
#include "usr_lin.h"
#include "usr_screen.h"

uint8_t g_mcuTemp;
volatile uint32_t systemTimer;
int datacheck;

void UsrSystemInit(void)
{
  HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
	
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);
	
	HAL_TIM_Base_Start_IT(&htim16);
	
	UsrScreenInit();
	SetMainScreen();
}

void UsrSystemGeneral(void)
{
	UpdateMainScreen();
	UsrLinTxProccess();
	UsrLinRxProccess();

	if(datacheck)
	{
	datacheck=0;
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
		UsrDelay(100);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
		UsrDelay(100);
		g_mcuTemp=GetMcuTemp();
	}
}