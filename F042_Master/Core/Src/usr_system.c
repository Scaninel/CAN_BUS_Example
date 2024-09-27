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
float frequency;
float g_rpmMeasured;
volatile uint8_t captureDone;

float error, control, prevError;

float Kp = 0.01229555110361;
float Ki = 0.686283392874;
float Kd = 3.730893251148e-05;

float integral;
float prevMeasured;

void MotorControlProc(void);

void UsrSystemInit(void)
{
  HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);
	
//	HAL_HalfDuplex_EnableReceiver(&huart1);
//	HAL_UART_Receive_IT(&huart1, &g_LinSingleMsg, 1);
	
	HAL_TIM_Base_Start_IT(&htim16);
	
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	
	HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, &ICValue, 1);
}

void UsrSystemGeneral(void)
{
	CAN_DataCheck();
	LINProc();
	MotorControlProc();
}

void MotorControlProc(void)
{
	if (captureDone)  // Check if a new capture is done
	{
		if (ICValue != 0)
		{
			frequency = 1000000.0 / ICValue;  // Calculate the frequency
			captureDone = 0;  // Reset the flag for the next capture
			g_rpmMeasured = frequency*60/PPR;
		}
	}
	
	if(!(systemTimer % PID_TASK_SAMPLE_TIME_MS))
	{
		error = g_rpmRequest - g_rpmMeasured;
	
		if(g_motorEnbl)
		{
			// Integral term with windup guarding
			integral += Ki * error * PID_TASK_SAMPLE_TIME_S;
			if (integral > 100) integral = 100;
			else if (integral < 0) integral = 0;
			
			// Derivative term
      double derivative = (g_rpmMeasured - prevMeasured) / PID_TASK_SAMPLE_TIME_S;
			
			// PID control output
			control = Kp * error + integral - Kd * derivative;

			// Store current error and measurement for next iteration
			prevError = error;
			prevMeasured = g_rpmMeasured;
		}
		else
		{
			control = 0;
			integral = 0;
			prevError = 0;
			prevMeasured = 0;
		}

		// Control output limit
		if (control > 100)
			control = 100;
		else if (control < 0)
			control = 0;
		
		// Assign dutyc cycle for the PWM output
		TIM3->CCR3 = control;
	}
}
