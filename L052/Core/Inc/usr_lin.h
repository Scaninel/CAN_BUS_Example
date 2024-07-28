#include "main.h"
#include "string.h"

#define LIN_BUFFER_LEN 	8
#define LIN_TEMP_ID 		47

HAL_StatusTypeDef UsrLIN_HeaderTx(const uint8_t id);
HAL_StatusTypeDef UsrLIN_ResponseTx(const uint8_t *data, size_t dataSize);
void UsrLedBlink(const uint32_t timeout, const uint8_t blinkCount);
void UsrDelay(const uint32_t timeout);
void UsrLinRxProccess(void);
void UsrLinTxProccess(void);

extern volatile uint8_t g_LIN_MsgReceived;
extern uint8_t LinDataRxLenght;
extern uint8_t LinRxBuf[LIN_BUFFER_LEN];

extern uint8_t g_LIN_TempRxFlg;
extern uint8_t g_Received_Temp;
extern uint8_t g_Received_CanSt;