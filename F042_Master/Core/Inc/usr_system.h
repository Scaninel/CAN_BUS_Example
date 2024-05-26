#include "main.h"

#define true 1
#define false 0

void UsrSystemInit(void);
void UsrSystemGeneral(void);
void UsrLedBlink(const uint32_t timeout, const uint8_t blinkCount);
void UsrDelay(const uint32_t timeout);


extern uint8_t g_mcuTemp;
extern int datacheck;
extern uint8_t g_mcuTemp;
extern volatile uint32_t systemTimer;