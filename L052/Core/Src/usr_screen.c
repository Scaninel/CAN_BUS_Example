#include "usr_system.h"
#include "usr_screen.h"
#include "usr_lin.h"
#include "fonts.h"
#include "ssd1306.h"
#include <stdio.h>

void initialScreenMessage(char *msg);

uint8_t writtenCanStat = false;
uint8_t writtenTemp = 0;
uint8_t writtenLinStat = false;

void UsrScreenInit(void)
{
	ssd1306_Init();
	initialScreenMessage("Device is started");
}

void initialScreenMessage(char *msg)
{
    ssd1306_WriteString(msg,Font_7x10,White);

    ssd1306_UpdateScreen();
		HAL_Delay(2000);
		ssd1306_Fill(Black);
		ssd1306_UpdateScreen();
}

void SetMainScreen(void)
{
	//writtenTemp = GetMcuTemp();
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("CAN :-",Font_7x10,White);
	ssd1306_SetCursor(0, 10);
	ssd1306_WriteString("LIN :-",Font_7x10,White);
	ssd1306_SetCursor(0, 20);
	ssd1306_WriteString("Temp:-",Font_7x10,White);
	ssd1306_UpdateScreen();
}

void UpdateMainScreen(void)
{
	if(writtenTemp != Received_Temp)
	{
		ssd1306_Fill(Black);
		writtenTemp = Received_Temp;;
		ssd1306_SetCursor(0, 0);
		
		char str_writtenTemp[10];

		sprintf(str_writtenTemp, "%d", writtenTemp);
		
//		if(g_CANstat)
//			ssd1306_WriteString("CAN :ON",Font_7x10,White);
//		else
//			ssd1306_WriteString("CAN :OFF",Font_7x10,White);
//		
//		ssd1306_SetCursor(0, 10);
//		if(g_LinStat)
//			ssd1306_WriteString("LIN :ON",Font_7x10,White);
//		else
//			ssd1306_WriteString("LIN :OFF",Font_7x10,White);
		
		ssd1306_WriteString("CAN :-",Font_7x10,White);
		ssd1306_SetCursor(0, 10);
		ssd1306_WriteString("LIN :-",Font_7x10,White);
		ssd1306_SetCursor(0, 20);
		
		ssd1306_SetCursor(0, 20);
		ssd1306_WriteString("Temp:",Font_7x10,White);
		ssd1306_WriteString(str_writtenTemp,Font_7x10,White);
		ssd1306_UpdateScreen();
	}
}