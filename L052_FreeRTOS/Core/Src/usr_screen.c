/*
 * usr_screen.c
 *
 *  Created on: Oct 6, 2024
 *      Author: Can
 */
#include "main.h"
#include "cmsis_os.h"
#include "usr_screen.h"
#include "fonts.h"
#include "ssd1306.h"
#include "usr_lin.h"

uint8_t writtenCanStat = NTW_INIT;
uint8_t writtenLinStat = NTW_INIT;
uint8_t writtenTemp;
uint32_t writtenRpm;

void ScreenInit(void)
{
	ssd1306_Init();

	ssd1306_WriteString("Device is started",Font_7x10,White);
	ssd1306_UpdateScreen();
	osDelay(250);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();

	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("CAN :-",Font_7x10,White);
	ssd1306_SetCursor(0, 10);
	ssd1306_WriteString("LIN :-",Font_7x10,White);
	ssd1306_SetCursor(0, 20);
	ssd1306_WriteString("Temp:-",Font_7x10,White);
	ssd1306_UpdateScreen();
}

void ScreenLoop(void)
{
	if((writtenTemp != g_Received_Temp) || (writtenCanStat != g_Received_CanSt) || (writtenLinStat != g_Received_LinSt) || (writtenRpm != g_Received_rpm))
	{
		ssd1306_Fill(Black);
		writtenTemp = g_Received_Temp;
		writtenCanStat = g_Received_CanSt;
		writtenLinStat = g_Received_LinSt;
		writtenRpm = (uint32_t)g_Received_rpm;
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
		ssd1306_WriteString(str_writtenTemp,Font_7x10,White);

		ssd1306_UpdateScreen();
	}
}
