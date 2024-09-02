#include "usr_system.h"
#include "usr_lin.h"

#define CAN_TEMP_TX_PERIOD 10000
#define CAN_SPEED_TX_PERIOD 500
#define CAN_MAX_DATA_LENGTH 8
#define CAN_TEMP_ID 11
#define CAN_REF_SPEED_ID 22

CAN_TxHeaderTypeDef CAN_TxHeader;
uint8_t CAN_TxData[8];
uint32_t CAN_TxMailbox;
volatile uint32_t canTimer;

HAL_StatusTypeDef UsrCanTempTxProccess(uint8_t *data, uint8_t data_size)
{
	//transmit data in TX period
	if (!(canTimer % CAN_TEMP_TX_PERIOD))
	{		
		CAN_TxHeader.DLC = data_size;  // data length
		CAN_TxHeader.IDE = CAN_ID_STD;
		CAN_TxHeader.RTR = CAN_RTR_DATA;
		CAN_TxHeader.StdId = CAN_TEMP_ID;  // ID
		
		memcpy(CAN_TxData, data, data_size);
		
		return HAL_CAN_AddTxMessage(&hcan,&CAN_TxHeader,CAN_TxData,&CAN_TxMailbox);
	}
	
	return HAL_TIMEOUT;
}

HAL_StatusTypeDef UsrCanRefSpeedTxProccess(uint8_t *data, uint8_t data_size)
{
	//transmit data in TX period
	if (!(canTimer % CAN_SPEED_TX_PERIOD))
	{		
		CAN_TxHeader.DLC = data_size;  // data length
		CAN_TxHeader.IDE = CAN_ID_STD;
		CAN_TxHeader.RTR = CAN_RTR_DATA;
		CAN_TxHeader.StdId = CAN_REF_SPEED_ID;  // ID
		
		memcpy(CAN_TxData, data, data_size);
		
		return HAL_CAN_AddTxMessage(&hcan,&CAN_TxHeader,CAN_TxData,&CAN_TxMailbox);
	}
	
	return HAL_TIMEOUT;
}