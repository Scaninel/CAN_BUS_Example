/*
 * usr_lin.h
 *
 *  Created on: Oct 6, 2024
 *      Author: Can
 */

#ifndef INC_USR_LIN_H_
#define INC_USR_LIN_H_

#define LIN_BUFFER_LEN 	13
#define LIN_TEMP_ID 		47

#define NTW_CAN_BIT (1 << 0)
#define NTW_LIN_BIT (1 << 1)

#define LIN_HEADER_LEN 4
#define LIN_SYNC_BREAK_1 0x00
#define LIN_SYNC_BREAK_2 0x80
#define LIN_SYNC_FIELD 0x55

#define LIN_TEMP_WR_ID 	7
#define LIN_TEMP_R_ID 	77
#define LIN_NTW_ST_ID 	55
#define LIN_RPM_WR 			99

#define true 1
#define false 0

void UsrLinRxCallback(void);

extern uint8_t g_LIN_TempRxFlg;
extern uint8_t g_Received_Temp;
extern uint8_t g_Received_CanSt;
extern uint8_t g_Received_LinSt;
extern float g_Received_rpm;
extern volatile uint8_t g_LinIdle;
extern uint8_t g_LinSingleMsg;

void LinLoop(void);

#endif /* INC_USR_LIN_H_ */
