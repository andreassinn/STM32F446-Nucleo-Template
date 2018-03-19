[#ftl]
[#assign coreDir=sourceDir]
[#list configs as dt]
[#assign data = dt]
[#assign peripheralParams =dt.peripheralParams]
[#assign usedIPs =dt.usedIPs]
/**
  ******************************************************************************
  * @file           : fram.c
  * @brief          : Sourcefile for the fram driver
  ******************************************************************************
*/

#include "board_utils.h"
[#if isHalSupported?? && isHALUsed?? ]
#include "${FamilyName?lower_case}xx_hal.h"
[/#if]
#include "string.h"
#include "main.h"

static uint8_t resetFlag = 0;
[#list usedIPs as ip ]
	[#if ip?contains("I2C1")]
		[#list peripheralParams.get(ip).entrySet() as paramEntry]
			[#if paramEntry.key?contains("Instance")]
				[#assign I2CInstance = paramEntry.value?replace("$Index", "1")]
				[#assign I2CHandle = I2CInstance?lower_case]
			[/#if]
		[/#list]

[#assign i2cHandleVar = "h${I2CHandle}"]

extern I2C_HandleTypeDef ${i2cHandleVar};
extern void MX_${I2CInstance}_Init(void);

#ifndef MEM_ADDRESS
#define MEM_ADDRESS 0x0A
#endif

static uint8_t memError = 0;

#if BLOCKING_MODE != 1
#ifndef FRAM_BUFFER_SIZE
#define FRAM_BUFFER_SIZE 1000
#endif
static uint8_t writeBuffer[FRAM_BUFFER_SIZE];
#endif

static fram_sys status;

//=============================================Private Functions===========================================================================//

static uint8_t getI2CAddress(uint8_t page)
{
	return ((MEM_ADDRESS << 3) | (page & 0x07)) << 1;
}

static int fram_write_private(uint8_t page, uint16_t address, void* data, size_t dataLength)
{
#if BLOCKING_MODE != 1
	HAL_StatusTypeDef ret;
#endif
#if BLOCKING_MODE == 1
	return HAL_I2C_Mem_Write(&${i2cHandleVar}, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength, FRAM_BLOCK_TIMEOUT) == HAL_OK;
#else
	if(dataLength > FRAM_BUFFER_SIZE)
		return -2;
	while(fram_busy())
		;
	memcpy(writeBuffer, data, dataLength);
	if((&${i2cHandleVar})->hdmatx != 0)
		ret = HAL_I2C_Mem_Write_DMA(&${i2cHandleVar}, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, writeBuffer, dataLength);
	else if(__HAL_Interrupt_Is_Enabled(${I2CInstance}_EV_IRQn))
		ret = HAL_I2C_Mem_Write_IT(&${i2cHandleVar}, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength);
	else
		ret = HAL_I2C_Mem_Write(&${i2cHandleVar}, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength, FRAM_BLOCK_TIMEOUT);
	return ret == HAL_OK;
#endif

}

static int fram_read_private(uint8_t page, uint16_t address, void* data, size_t dataLength)
{
	while(fram_busy())
			;
	return HAL_I2C_Mem_Read(&${i2cHandleVar}, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength, FRAM_BLOCK_TIMEOUT) == HAL_OK;
}

#if BLOCKING_MODE != 1
static int fram_read_private_noblock(uint8_t page, uint16_t address, void* data, size_t dataLength)
{
	while(fram_busy())
			;
	if((&${i2cHandleVar})->hdmarx != 0)
		return HAL_I2C_Mem_Read_DMA(&${i2cHandleVar}, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength) == HAL_OK;
	else if(__HAL_Interrupt_Is_Enabled(${I2CInstance}_EV_IRQn))
		return HAL_I2C_Mem_Read_IT(&${i2cHandleVar}, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength) == HAL_OK;
	else
		return HAL_I2C_Mem_Read(&${i2cHandleVar}, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength, FRAM_BLOCK_TIMEOUT) == HAL_OK;
}
#endif

//=============================================Public Functions===========================================================================//

//===============================================FRAM Interface===========================================================================//
void MX_FRAM_Init()
{
	fram_init();
}

void fram_init()
{
	if (HAL_I2C_GetState(&${i2cHandleVar}) == HAL_I2C_STATE_RESET) {
		MX_${I2CInstance}_Init();
  	}

	if(resetFlag == 0)
		system_init();

	memset(&status, 0x00, sizeof(fram_sys));
#if FRAM_CLEAR_ON_INIT == 1
	memError |= !fram_write_private(0, 0, &status, sizeof(fram_sys));
#endif
	memError |= !fram_read_private(0, 0, &status, sizeof(fram_sys));
	if(!status.flags.intialized)
	{
		status.flags.intialized = 1;
		memcpy(&(status.name), MODULE_NAME, strlen(MODULE_NAME));
		memError |= !fram_write_private(0, 0, &status, sizeof(fram_sys));
		memset(&status, 0x00, sizeof(fram_sys));
	}
	memError |= !fram_read_private(0, 0, &status, sizeof(fram_sys));

	status.totalReset_cycles++;

	switch(getResetFlag())
	{
		case RCC_FLAG_BORRST:
			status.borResets++;
			break;
		case RCC_FLAG_PINRST:
			status.pinResets++;
			break;
		case RCC_FLAG_PORRST:
			status.porResets++;
			break;
		case RCC_FLAG_SFTRST:
			status.softwareResets++;
			break;
		case RCC_FLAG_IWDGRST:
			status.independendWatchdogResets++;
			break;
		case RCC_FLAG_WWDGRST:
			status.windowedWatchdogResets++;
			break;
		case RCC_FLAG_LPWRRST:
			status.lowPowerResets++;
			break;
		default:
			break;
	}

	memError |= !fram_write_private(0, 0, &status, sizeof(fram_sys));
}

int fram_busy()
{
	return hi2c1.State != HAL_I2C_STATE_READY;
}

int fram_write(uint8_t page, uint16_t address, void* data, size_t dataLength)
{
	if(memError)
		return 0;
	return fram_write_private(page + 1, address, data, dataLength);
}

int fram_read(uint8_t page, uint16_t address, void* data, size_t dataLength)
{
	if(memError)
		return 0;
	return fram_read_private(page + 1, address, data, dataLength);
}

int fram_read_noblock(uint8_t page, uint16_t address, void* data, size_t dataLength)
{
	if(memError)
		return 0;
#if BLOCKING_MODE != 1
	return fram_read_private_noblock(page + 1, address, data, dataLength);
#else
	return fram_read_private(page + 1, address, data, dataLength);
#endif
}

fram_sys* fram_getSystemStatus()
{
	return &status;
}
	[/#if]
[/#list]
[/#list]
//===============================================System Interface===========================================================================//

void system_init()
{
	if(__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
		resetFlag = RCC_FLAG_BORRST;
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
		resetFlag = RCC_FLAG_PINRST;
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
		resetFlag = RCC_FLAG_PORRST;
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
		resetFlag = RCC_FLAG_SFTRST;
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
		resetFlag = RCC_FLAG_IWDGRST;
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
		resetFlag = RCC_FLAG_WWDGRST;
	else if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
		resetFlag = RCC_FLAG_LPWRRST;
	else
		resetFlag = 0;

	__HAL_RCC_CLEAR_RESET_FLAGS();
}

int getSystemID(char* buffer, uint32_t bufferSize)
{
	if(bufferSize < SYSTEM_ID_STRING_SIZE)
	{
		return 0;
	}
	return sprintf(buffer,"%s: %lX%lX%lX",SYSTEM_NAME,STM32_UUID[0],STM32_UUID[1],STM32_UUID[2]);
}

uint8_t getResetFlag()
{
	return resetFlag;
}

/************************ (C) COPYRIGHT Peter Kremsner ${year} *****END OF FILE****/