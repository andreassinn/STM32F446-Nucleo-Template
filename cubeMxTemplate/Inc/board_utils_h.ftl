[#ftl]
[#list configs as dt]
[#assign usedIPs = dt.usedIPs]
/**
  ******************************************************************************
  * @file           : fram.h
  * @brief          : header file for the fram driver
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FRAM_H_
#define FRAM_H_

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include "main.h"
[#if isHalSupported?? && isHALUsed?? ]
#include "${FamilyName?lower_case}xx_hal.h"
[/#if]

#define BOARD_UTILITIES_IN_USE 1
[#list usedIPs as ip]
	[#if ip?contains("I2C1")]

#ifndef BLOCKING_MODE
#define BLOCKING_MODE 0
#endif

#ifndef FRAM_BLOCK_TIMEOUT
#define FRAM_BLOCK_TIMEOUT 1000
#endif

#ifndef FRAM_CLEAR_ON_INIT
#define FRAM_CLEAR_ON_INIT 0
#endif

#define BOARD_FRAM_IN_USE 1

typedef struct
{
	char name[15];
	struct
	{
		unsigned int intialized :1;
		unsigned int reserved :7;
	} flags;
	uint32_t totalReset_cycles;
	uint16_t borResets;
	uint16_t pinResets;
	uint16_t porResets;
	uint16_t softwareResets;
	uint16_t independendWatchdogResets;
	uint16_t windowedWatchdogResets;
	uint16_t lowPowerResets;
} fram_sys;

//=================================================================Fram Interface===========================================================/

void MX_FRAM_Init();
void fram_init();
int fram_busy();
int fram_write(uint8_t page, uint16_t address, void* data, size_t dataLength);
int fram_read(uint8_t page, uint16_t address, void* data, size_t dataLength);
int fram_read_noblock(uint8_t page, uint16_t address, void* data, size_t dataLength);

fram_sys* fram_getSystemStatus();
	[/#if]
[/#list]
[/#list]

//=================================================================System Interface=========================================================/

#define BOARD_SYS_IN_USE 1

#ifndef MODULE_NAME
#define MODULE_NAME "Mechatronik"
#endif

void system_init();
int getSystemID(char* buffer, uint32_t bufferSize);
uint8_t getResetFlag();

#define STM32_UUID ((uint32_t *)0x1FFF7A10)
#ifdef BOARD_FRAM_IN_USE
#define SYSTEM_NAME ((fram_getSystemStatus())->name)
#define SYSTEM_ID_STRING_SIZE (sizeof(((fram_sys*)0)->name) + 2 + 25)
#else
#define SYSTEM_NAME "No fram in use"
#define SYSTEM_ID_STRING_SIZE (25 + 14)
#endif

//=================================================================Some usefull macros======================================================/

#define __HAL_TIM_SET_DUTYCYCLE(htim, channel, dutycycle) (__HAL_TIM_SET_COMPARE(htim, channel, __HAL_TIM_GET_AUTORELOAD(htim) * (dutycycle > 100 ? 100 : dutycycle)/100))
#define __HAL_TIM_GET_DUTYCYCLE(htim, channel) (__HAL_TIM_GET_COMPARE(htim, channel) / (__HAL_TIM_GET_AUTORELOAD(htim) * 100))

#define __HAL_Interrupt_Is_Enabled(IRQn) (NVIC->ISER[(uint32_t)((int32_t)IRQn) >> 5] & (uint32_t)(1 << ((uint32_t)((int32_t)IRQn) & (uint32_t)0x1F)))

#ifdef __cplusplus
}
#endif

#endif
/************************ (C) COPYRIGHT Peter Kremsner ${year} *****END OF FILE****/
