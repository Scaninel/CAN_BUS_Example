#include "main.h"

#define true 1
#define false 0
	
#define MAX_MOTOR_SPEED 10000

void UsrSystemInit(void);
void UsrSystemGeneral(void);

extern uint8_t g_mcuTemp;
extern int datacheck;
extern uint8_t g_mcuTemp;
extern volatile uint32_t systemTimer;
extern volatile uint8_t captureDone;
extern float g_rpmMeasured;
extern uint8_t g_MotorStTx;