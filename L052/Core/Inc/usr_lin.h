#include "main.h"
#include "string.h"

#define LIN_BUFFER_LEN 	13
#define LIN_TEMP_ID 		47

#define NTW_CAN_BIT (1 << 0)
#define NTW_LIN_BIT (1 << 1)

HAL_StatusTypeDef UsrLIN_HeaderTx(const uint8_t id);
HAL_StatusTypeDef UsrLIN_ResponseTx(const uint8_t *data, size_t dataSize);
void UsrLedBlink(const uint32_t timeout, const uint8_t blinkCount);
void UsrDelay(const uint32_t timeout);
void UsrLinRxCallback(void);
void UsrLinRxProccess(void);
void UsrLinTxProccess(void);

extern volatile uint8_t g_LIN_MsgReceived;
extern uint8_t LinRxBuf[LIN_BUFFER_LEN];

extern uint8_t g_LIN_TempRxFlg;
extern uint8_t g_Received_Temp;
extern uint8_t g_Received_CanSt;
extern uint8_t g_Received_LinSt;
extern float g_Received_rpm;

extern uint8_t g_LinSingleMsg;
extern volatile uint8_t g_LinIdle;