#include "usr_system.h"
#include "usr_screen.h"
#include "usr_lin.h"
#include "fonts.h"
#include "ssd1306.h"
#include <stdio.h>

void initialScreenMessage(char *msg);

uint8_t writtenCanStat = NTW_INIT;
uint8_t writtenLinStat = NTW_INIT;
uint8_t writtenTemp = 0;

void UsrScreenInit(void)
{
	ssd1306_Init();
	initialScreenMessage("Device is started");
}

void initialScreenMessage(char *msg)
{
    ssd1306_WriteString(msg,Font_7x10,White);

    ssd1306_UpdateScreen();
		//HAL_Delay(2000);
		ssd1306_Fill(Black);
		ssd1306_UpdateScreen();
}

void SetMainScreen(void)
{
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
	if((writtenTemp != g_Received_Temp) || (writtenCanStat != g_Received_CanSt) || (writtenLinStat != g_Received_LinSt))
	{
		ssd1306_Fill(Black);
		writtenTemp = g_Received_Temp;
		writtenCanStat = g_Received_CanSt;
		writtenLinStat = g_Received_LinSt;
		ssd1306_SetCursor(0, 0);

		if (writtenCanStat == NTW_ON)
			ssd1306_WriteString("CAN :On",Font_7x10,White);
		else if (writtenCanStat == NTW_OFF)
			ssd1306_WriteString("CAN :Off",Font_7x10,White);
		else
			ssd1306_WriteString("CAN :-",Font_7x10,White);
		
		ssd1306_SetCursor(0, 10);
		if (writtenLinStat == NTW_ON)
			ssd1306_WriteString("LIN :On",Font_7x10,White);
		else if (writtenLinStat == NTW_OFF)
			ssd1306_WriteString("LIN :Off",Font_7x10,White);
		else
			ssd1306_WriteString("LIN :-",Font_7x10,White);
						
		char str_writtenTemp[10];
		sprintf(str_writtenTemp, "%d", writtenTemp);
		ssd1306_SetCursor(0, 20);
		ssd1306_WriteString("Temp:",Font_7x10,White);
		if(g_LIN_TempRxFlg)
			ssd1306_WriteString(str_writtenTemp,Font_7x10,White);
		else
			ssd1306_WriteString("-",Font_7x10,White);
		ssd1306_UpdateScreen();
	}
}