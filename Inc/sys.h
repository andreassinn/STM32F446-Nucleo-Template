/*
 * sys.h
 *
 *  Created on: 12.03.2018
 *      Author: peter
 */

#ifndef SYS_H_
#define SYS_H_

#include "fram.h"
#include <inttypes.h>
#include "stm32f4xx_hal.h"

#define STM32_UUID ((uint32_t *)0x1FFF7A10)
#define SYSTEM_NAME ((fram_getSystemStatus())->name)
#define SYSTEM_ID_STRING_SIZE (sizeof(((fram_sys*)0)->name) + 2 + 25)

void sysStartup();
int getSystemID(char* buffer, uint32_t bufferSize);
uint8_t getResetFlag();

#endif /* SYS_H_ */
