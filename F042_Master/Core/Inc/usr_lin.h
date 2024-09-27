#include "main.h"

#define LIN_BUFFER_LEN 	9

HAL_StatusTypeDef LIN_TxMessage(const uint8_t *data, const uint8_t data_size);

void LINProc(void);
HAL_StatusTypeDef PRO_LIN_TxHeaderData(uint8_t id, const uint8_t *data, uint8_t data_length);
HAL_StatusTypeDef PRO_LIN_TxHeader(uint8_t id);
void UsrLinRxProccess(void);
void UsrLinRxCallback(void);

extern uint8_t LinRxBuf[LIN_BUFFER_LEN];
extern volatile uint8_t g_LIN_MsgReceived;
extern uint8_t LinDataRxLenght;

extern uint8_t g_LinSingleMsg;
extern volatile uint8_t g_LinIdle;