#include "main.h"
#include "usr_screen.h"

#define true 1
#define false 0

void UsrSystemInit(void);
void UsrSystemGeneral(void);

extern volatile uint32_t systemTimer;

typedef union U_CONVERTER_TAG
{
    uint8_t buf[4];
    uint16_t u16val;
    uint32_t uival;
    int32_t i32val;
    uint32_t ui32val;
    int32_t ival;
    float fval;
}U_CONVERTER;