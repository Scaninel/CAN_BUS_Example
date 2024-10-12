#include "usr_can.h"

#define CAN_MAX_MSG_TO 30000

CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

CAN_TxHeaderTypeDef CAN_TxHeader;
uint8_t CAN_TxData[2];
uint32_t CAN_TxMailbox;

volatile uint8_t g_CANstat;
volatile uint8_t g_CAN_DataReceived;
volatile uint32_t g_CAN_Timer;

uint8_t g_LIN_TempTx;
uint8_t g_NetworkStTx;

uint8_t g_NetworkSt;
uint8_t g_receivedTemp;
uint16_t g_rpmRequest;
uint8_t g_motorEnbl;

HAL_StatusTypeDef CAN_DataCheck(void)
{
	// CAN STATUS CHECK
	if (g_CAN_Timer > CAN_MAX_MSG_TO)
	{
		g_NetworkStTx = true;
		g_NetworkSt &= ~NTW_CAN_BIT;
	}
	
	// CAN MSG PROCESS
	if(g_CAN_DataReceived)
	{
		g_CAN_DataReceived=0;
		g_CAN_Timer = 0;
		
		g_NetworkStTx = true;
		g_NetworkSt |= NTW_CAN_BIT;
		
		if ((RxHeader.StdId == CAN_TEMP_ID) && (g_receivedTemp != RxData[0]))
		{
			g_receivedTemp = RxData[0];
			g_LIN_TempTx = true;
			return HAL_OK;
		}
		else if(RxHeader.StdId == CAN_REF_SPEED_ID)
		{
			uint16_t f_rpmRequestRx = MAX_MOTOR_SPEED * RxData[0] / 100;
			
			if(g_motorEnbl != RxData[1] || f_rpmRequestRx != g_rpmRequest)
				g_MotorStTx = true;
			
			g_rpmRequest = f_rpmRequestRx;			
			g_motorEnbl = RxData[1];
			
			g_motorEnbl ? HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET): HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
		}
	}
	return HAL_BUSY;
}