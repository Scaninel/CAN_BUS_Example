#include "main.h"

#define MAX_LIN_DATA_BYTE 8
#define _LIN_BUFFER_SIZE 256

HAL_StatusTypeDef LIN_TxMessage(const uint8_t *data, const uint8_t data_size);

void LINProc(void);
HAL_StatusTypeDef PRO_LIN_TxHeaderData(uint8_t id, const uint8_t *data, uint8_t data_length);
HAL_StatusTypeDef PRO_LIN_TxHeader(uint8_t id);
void UsrLinRxProccess(void);
void UsrLIN_RxCallback(void);

extern uint8_t LIN_SingleData[4];
