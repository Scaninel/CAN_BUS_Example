#include "main.h"

#define CAN_ON   1
#define CAN_OFF  0
#define CAN_INIT 2

void UsrScreenInit(void);
void SetMainScreen(void);
void UpdateMainScreen(void);

extern uint8_t writtenTemp;