#include "usr_lin.h"

//HAL_StatusTypeDef UsrCanTxProccess(void);

extern CAN_RxHeaderTypeDef RxHeader;
extern uint8_t RxData[8];
volatile extern uint8_t g_CANstat;
volatile extern uint8_t g_CAN_DataReceived;

void CAN_DataCheck(void);
