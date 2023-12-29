#include "usr_system.h"
#include "usr_can.h"
#include "usr_lin.h"

volatile uint32_t systemTimer;
volatile uint32_t canTimer;

void UsrSystemInit(void)
{
	HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
	HAL_CAN_Start(&hcan);
}

void UsrSystemGeneral(void)
{
	UsrCanTxProccess();
}