#include "usr_system.h"
#include "usr_can.h"
#include "usr_lin.h"

uint8_t g_mcuTemp;
volatile uint32_t systemTimer;

HAL_StatusTypeDef stat;

void UsrSystemInit(void)
{
  HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
	
	//HAL_HalfDuplex_EnableReceiver(&huart1);
	//HAL_UART_Receive_IT(&huart1,&LIN_SingleData,1);
	
	//HAL_TIM_Base_Start_IT(&htim16);
}

uint8_t data = 15;
void UsrSystemGeneral(void)
{
	//UsrLinTxProccess();
	//UsrLinRxProccess();
	
	stat = CAN_DataCheck();
	
//	PRO_LIN_TxHeaderData(7, &data,1);
//	data++;
//	HAL_Delay(2000);
//	PRO_LIN_TxHeader(77);
//	HAL_Delay(5000);
}
