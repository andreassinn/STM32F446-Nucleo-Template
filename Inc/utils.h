/*
 * utils.h
 *
 *  Created on: 07.03.2018
 *      Author: peter
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "stm32f4xx_hal.h"

#define __HAL_TIM_SET_DUTYCYCLE(htim, channel, dutycycle) (__HAL_TIM_SET_COMPARE(htim, channel, __HAL_TIM_GET_AUTORELOAD(htim) * (dutycycle > 100 ? 100 : dutycycle)/100))

#endif /* UTILS_H_ */
