#include "system.h"

void systemInit()
{
    ssd1306_Init();
    FDCAN_Config();
	initialScreenMessage();
}

void systemProc()
{

}