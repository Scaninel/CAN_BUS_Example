#include "main.h"

#define MAX_LIN_DATA_BYTE 8
#define _LIN_BUFFER_SIZE 256

void UsrLinRxProccess(void);
void UsrLIN_RxCallback(void);
HAL_StatusTypeDef UsrLIN_HeaderTx(const uint8_t id);
HAL_StatusTypeDef UsrLIN_ResponseTx(const uint8_t *data, size_t dataSize);
void UsrLedBlink(const uint32_t timeout, const uint8_t blinkCount);
void UsrDelay(const uint32_t timeout);
int8_t GetMcuTemp(void);

extern uint8_t LIN_SingleData;
extern volatile uint8_t g_LinHeaderRxCpltFlg;
extern volatile uint8_t g_LinResponseRxCpltFlg;
//extern volatile uint8_t g_LinTxButtonPressed;
extern uint8_t g_LinHeaderRxId;
extern uint8_t LinRxBuf[13];
extern volatile uint32_t LinTimer;
extern uint8_t LinDataRxLenght;
