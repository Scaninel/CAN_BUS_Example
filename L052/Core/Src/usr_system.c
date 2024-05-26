#include "usr_system.h"
#include "usr_lin.h"

volatile uint32_t systemTimer;

void UsrSystemInit(void)
{
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);
	
	HAL_TIM_Base_Start_IT(&htim6);
	UsrScreenInit();
	SetMainScreen();
}

void UsrSystemGeneral(void)
{
	UsrLinRxProccess();
	UpdateMainScreen();
}