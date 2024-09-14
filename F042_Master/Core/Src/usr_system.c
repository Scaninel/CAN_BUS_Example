#include "usr_system.h"
#include "usr_can.h"
#include "usr_lin.h"

#define PPR 11

uint8_t g_mcuTemp;
volatile uint32_t systemTimer;

HAL_StatusTypeDef stat;

uint32_t ICValue = 0;
double frequency = 0;
double rpm = 0;
volatile uint8_t captureDone;

void UsrSystemInit(void)
{
  HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
	
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, LinRxBuf, 8);
	
	HAL_TIM_Base_Start_IT(&htim16);
	
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	
	HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, &ICValue, 1);
}

void UsrSystemGeneral(void)
{
	CAN_DataCheck();
	LINProc();
	
	if (captureDone)  // Check if a new capture is done
	{
		if (ICValue != 0)
		{
			frequency = 1000000.0 / ICValue;  // Calculate the frequency
			captureDone = 0;  // Reset the flag for the next capture
			rpm = frequency*60/PPR;
		}
	}
	
//	PRO_LIN_TxHeaderData(7, &data,1);
//	data++;
//	HAL_Delay(2000);
//	PRO_LIN_TxHeader(77);
//	HAL_Delay(5000);
}
