/*
 * motor.c
 *
 *  Created on: 15.03.2018
 *      Author: peter
 */
#include "stm32f4xx_hal.h"
#include "tim.h"

static volatile uint8_t commutation_counter;
const static uint8_t commutationTable[6][6] =
{
{ 0, 1, 1, 0, 0, 0 },
{ 0, 1, 0, 0, 1, 0 },
{ 0, 0, 0, 1, 1, 0 },
{ 1, 0, 0, 1, 0, 0 },
{ 1, 0, 0, 0, 0, 1 },
{ 0, 0, 1, 0, 0, 1 } };

void motor_start()
{
	commutation_counter = 0;
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim3)
	{
		uint8_t BH1 = commutationTable[commutation_counter][0];
		uint8_t BL1 = commutationTable[commutation_counter][1];

		uint8_t BH2 = commutationTable[commutation_counter][2];
		uint8_t BL2 = commutationTable[commutation_counter][3];

		uint8_t BH3 = commutationTable[commutation_counter][4];
		uint8_t BL3 = commutationTable[commutation_counter][5];

		commutation_counter++;
		if(commutation_counter > 5)
			commutation_counter = 0;

		if(BH1)
		{
			//LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1N);
			//LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
		}
		else
		{
			//LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1);
			if(BL1)
			{
				//LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1N);
			}
			else
			{
				//LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1N);
			}
		}

		if(BH2)
		{
			//LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2N);
			//LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
		}
		else
		{
			//LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2);
			if(BL2)
			{
				//LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2N);
			}
			else
			{
				//LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2N);
			}
		}

		if(BH3)
		{
			//LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3N);
			//LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
		}
		else
		{
			//LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3);
			if(BL3)
			{
				//LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N);
			}
			else
			{
				//LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3N);
			}
		}
	}
}
