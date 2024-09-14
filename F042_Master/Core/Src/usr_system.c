#include "usr_system.h"
#include "usr_can.h"
#include "usr_lin.h"

#define PPR 11
#define PID_TASK_SAMPLE_TIME_MS 10
#define PID_TASK_SAMPLE_TIME_S 0.01

uint8_t g_mcuTemp;
volatile uint32_t systemTimer;

HAL_StatusTypeDef stat;

uint32_t ICValue;
double frequency;
double rpmMeasured;
volatile uint8_t captureDone;

double error, control, prevError;

double Kp = 0.01229555110361;
double Ki = 0.686283392874;
double Kd = 3.730893251148e-05;

double integral;
double prevMeasured;

void UsrSystemInit(void)
{
  HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
	
	HAL_HalfDuplex_EnableReceiver(&huart1);
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, LinRxBuf, 8);
	
	HAL_TIM_Base_Start_IT(&htim16);
	
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	
	HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, &ICValue, 1);
}

void UsrSystemGeneral(void)
{
	CAN_DataCheck();
	LINProc();
	
	if (captureDone)  // Check if a new capture is done
	{
		if (ICValue != 0)
		{
			frequency = 1000000.0 / ICValue;  // Calculate the frequency
			captureDone = 0;  // Reset the flag for the next capture
			rpmMeasured = frequency*60/PPR;
		}
	}
	
	if(!(systemTimer % PID_TASK_SAMPLE_TIME_MS))
	{
		error = g_rpmRequest - rpmMeasured;
	
		if(g_motorEnbl)
		{
			// Integral term with windup guarding
			integral += Ki * error * PID_TASK_SAMPLE_TIME_S;
			if (integral > 100) integral = 100;
			else if (integral < 0) integral = 0;
			
			// Derivative term
      double derivative = (rpmMeasured - prevMeasured) / PID_TASK_SAMPLE_TIME_S;
			
			// PID control output
			control = Kp * error + integral - Kd * derivative;

			// Store current error and measurement for next iteration
			prevError = error;
			prevMeasured = rpmMeasured;

		}
		else
		{
			control = 0;
			integral = 0;
			prevError = 0;
			prevMeasured = 0;
		}

		// Clamp control output
		if (control > 100)
			control = 100;
		else if (control < 0)
			control = 0;
			
		TIM3->CCR3 = control;
	}
	
	

	
	
	
	
//	PRO_LIN_TxHeaderData(7, &data,1);
//	data++;
//	HAL_Delay(2000);
//	PRO_LIN_TxHeader(77);
//	HAL_Delay(5000);
}
