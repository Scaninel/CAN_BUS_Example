#include "usr_lin.h"
#include "usr_system.h"

#define NTW_CAN_BIT (1 << 0)
#define NTW_LIN_BIT (1 << 1)

#define CAN_TEMP_ID 11
#define CAN_REF_SPEED_ID 22

//HAL_StatusTypeDef UsrCanTxProccess(void);

extern CAN_RxHeaderTypeDef RxHeader;
extern uint8_t RxData[8];
volatile extern uint8_t g_CANstat;
volatile extern uint8_t g_CAN_DataReceived;
extern volatile uint32_t g_CAN_Timer;


extern uint8_t g_LIN_TempTx;
extern uint8_t g_NetworkStTx;

extern uint8_t g_NetworkSt;
extern uint8_t g_receivedTemp;

HAL_StatusTypeDef CAN_DataCheck(void);
