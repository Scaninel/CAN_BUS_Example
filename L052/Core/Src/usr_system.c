#include "usr_system.h"
#include "usr_lin.h"

volatile uint32_t systemTimer;

void UsrSystemInit(void)
{
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, LinRxBuf, 8);
	
	HAL_TIM_Base_Start_IT(&htim6);
	
	UsrScreenInit();
	SetMainScreen();
}

void UsrSystemGeneral(void)
{
	UsrLinRxProccess();
	UpdateMainScreen();
}