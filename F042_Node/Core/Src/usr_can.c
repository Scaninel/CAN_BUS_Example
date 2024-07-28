#include "usr_system.h"
#include "usr_lin.h"

#define CAN_TX_PERIOD 10000
#define CAN_MAX_DATA_LENGTH 8

CAN_TxHeaderTypeDef CAN_TxHeader;
uint8_t CAN_TxData[8];
uint32_t CAN_TxMailbox;
volatile uint32_t canTimer;

HAL_StatusTypeDef UsrCanTxProccess(uint8_t *data, uint8_t data_size)
{
	//transmit data in TX period
	if (canTimer > CAN_TX_PERIOD)
	{
		canTimer = 0;
		
		CAN_TxHeader.DLC = data_size;  // data length
		CAN_TxHeader.IDE = CAN_ID_STD;
		CAN_TxHeader.RTR = CAN_RTR_DATA;
		CAN_TxHeader.StdId = 0x103;  // ID
		
		memcpy(CAN_TxData, data, data_size);
		
		return HAL_CAN_AddTxMessage(&hcan,&CAN_TxHeader,CAN_TxData,&CAN_TxMailbox);
	}
	
	return HAL_TIMEOUT;
}