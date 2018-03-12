/*
 * fram.c
 *
 *  Created on: 08.03.2018
 *      Author: peter
 */

#include "fram.h"
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "string.h"
#include "main.h"
#include "sys.h"

#define MEM_ADDRESS 0x0A

#define BUFFER_SIZE 1000

static uint8_t memError = 0;

#if BLOCKING_MODE != 1
static uint8_t writeBuffer[BUFFER_SIZE];
#endif

static fram_sys status;

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
	return HAL_I2C_Mem_Write(&hi2c1, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength, 1000) == HAL_OK;
#else
	if(dataLength > BUFFER_SIZE)
		return -2;
	while(fram_busy())
		;
	memcpy(writeBuffer, data, dataLength);
	ret = HAL_I2C_Mem_Write_DMA(&hi2c1, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, writeBuffer, dataLength);
	return ret == HAL_OK;
#endif

}

static int fram_read_private(uint8_t page, uint16_t address, void* data, size_t dataLength)
{
	while(fram_busy())
			;
	return HAL_I2C_Mem_Read(&hi2c1, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength, 1000) == HAL_OK;
}

#if BLOCKING_MODE != 1
static int fram_read_private_DMA(uint8_t page, uint16_t address, void* data, size_t dataLength)
{
	while(fram_busy())
			;
	return HAL_I2C_Mem_Read_DMA(&hi2c1, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength) == HAL_OK;
}
#endif

void fram_init()
{
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

int fram_read_DMA(uint8_t page, uint16_t address, void* data, size_t dataLength)
{
	if(memError)
		return 0;
#if BLOCKING_MODE != 1
	return fram_read_private_DMA(page + 1, address, data, dataLength);
#else
	return fram_read_private(page + 1, address, data, dataLength);
#endif
}

fram_sys* fram_getSystemStatus()
{
	return &status;
}
