#include "usr_can.h"


CAN_TxHeaderTypeDef CAN_TxHeader;
uint8_t CAN_TxData[2];
uint32_t CAN_TxMailbox;

HAL_StatusTypeDef UsrCanTxProccess(void)
{
	if (g_LinTxButtonPressed)
	{
		g_LinTxButtonPressed = false;
		
		HAL_Delay(50);
		
		CAN_TxHeader.DLC = 2;  // data length
		CAN_TxHeader.IDE = CAN_ID_STD;
		CAN_TxHeader.RTR = CAN_RTR_DATA;
		CAN_TxHeader.StdId = 0x103;  // ID
		
		CAN_TxData[0]=20;
		CAN_TxData[1]=47;		
		
		return HAL_CAN_AddTxMessage(&hcan,&CAN_TxHeader,CAN_TxData,&CAN_TxMailbox);

	}
	
	return HAL_BUSY;
}