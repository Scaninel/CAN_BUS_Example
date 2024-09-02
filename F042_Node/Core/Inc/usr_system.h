#include "main.h"

#define true 1
#define false 0

void UsrSystemInit(void);
void UsrSystemGeneral(void);

extern volatile uint32_t systemTimer;
extern uint32_t adcValues[4];
extern volatile uint8_t g_ADC_ValReady;
extern uint8_t motorEnbl;
