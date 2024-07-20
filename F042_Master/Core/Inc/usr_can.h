#include "usr_lin.h"
#include "usr_system.h"

//HAL_StatusTypeDef UsrCanTxProccess(void);

extern CAN_RxHeaderTypeDef RxHeader;
extern uint8_t RxData[8];
volatile extern uint8_t g_CANstat;
volatile extern uint8_t g_CAN_DataReceived;
extern volatile uint32_t g_CAN_Timer;


extern uint8_t g_LIN_TempTx;
extern uint8_t g_LIN_CanStTx;

extern uint8_t g_CanSt;
extern uint8_t g_receivedTemp;

HAL_StatusTypeDef CAN_DataCheck(void);
