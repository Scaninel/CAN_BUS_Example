#include "main.h"
#include "string.h"

#define LIN_BUFFER_LEN 	13
#define LIN_TEMP_ID 		47


void UsrLIN_RxCallback(void);
HAL_StatusTypeDef UsrLIN_HeaderTx(const uint8_t id);
HAL_StatusTypeDef UsrLIN_ResponseTx(const uint8_t *data, size_t dataSize);
void UsrLedBlink(const uint32_t timeout, const uint8_t blinkCount);
void UsrDelay(const uint32_t timeout);
void UsrLinRxProccess(void);
void UsrLinTxProccess(void);

extern uint8_t LIN_SingleData;
extern volatile uint8_t g_LinHeaderRxCpltFlg;
extern volatile uint8_t g_LinResponseRxCpltFlg;
extern volatile uint8_t g_ButtonPressed;
extern volatile uint32_t LinTxTimer;
extern volatile uint32_t LinBusCheckTimer;
extern uint8_t LinDataRxLenght;
extern uint8_t LinRxBuf[LIN_BUFFER_LEN];

extern uint8_t Received_Temp;

extern volatile uint32_t g_LIN_Timeout;