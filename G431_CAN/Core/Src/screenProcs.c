#include "system.h"

void initialScreenMessage()
{
    ssd1306_WriteString("DEVICE IS STARTED!!",Font_7x10,White);

    ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
}