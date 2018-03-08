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

#define MEM_ADDRESS 0x0A
#define MEM_OFFSET 5

#define BUFFER_SIZE 1000

static uint8_t memError = 0;

#if BLOCKING_MODE != 1
static uint8_t writeBuffer[BUFFER_SIZE];
#endif

static sys_status status;
static sys_info info;

static uint8_t getI2CAddress(uint8_t page)
{
	return ((MEM_ADDRESS << 3) | (page & 0x07)) << 1;
}

static int fram_write_private(uint8_t page, uint8_t address, void* data, size_t dataLength)
{
#if BLOCKING_MODE != 1
	HAL_StatusTypeDef ret;
#endif
	if(page * 256 + address + dataLength > info.memAvailable)
		return -1;

#if BLOCKING_MODE == 1
	return HAL_I2C_Mem_Write(&hi2c1, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength, 1000) == HAL_OK;
#else
	if(dataLength > BUFFER_SIZE)
		return -2;
	memcpy(writeBuffer, data, dataLength);
	while((ret = HAL_I2C_Mem_Write_DMA(&hi2c1, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, writeBuffer, dataLength)) == HAL_BUSY);
	return ret == HAL_OK;
#endif

}

static int fram_read_private(uint8_t page, uint8_t address, void* data, size_t dataLength)
{
	if(page * 256 + address + dataLength > info.memAvailable)
			return -1;

	return HAL_I2C_Mem_Read(&hi2c1, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength, 1000) == HAL_OK;
}

#if BLOCKING_MODE != 1
static int fram_read_private_DMA(uint8_t page, uint8_t address, void* data, size_t dataLength)
{
	if(page * 256 + address + dataLength > info.memAvailable)
		return -1;

	return HAL_I2C_Mem_Read_IT(&hi2c1, getI2CAddress(page), address, I2C_MEMADD_SIZE_8BIT, data, dataLength) == HAL_OK;
}
#endif

void fram_init()
{

#if FRAM_CLEAR_ON_INIT == 1
	memset(&status, 0x00, sizeof(sys_status));
	memset(&info, 0x00, sizeof(sys_info));
	info.memAvailable = FRAM_SIZE;
	memcpy(&(info.name), MODULE_NAME, strlen(MODULE_NAME));

	memError |= !fram_write_private(0,MEM_OFFSET, &info, sizeof(sys_info));
	memError |= !fram_write_private(0,MEM_OFFSET + sizeof(sys_info),&status,sizeof(sys_status));
#else
	if(info.memAvailable == 0)
		info.memAvailable = FRAM_SIZE; // Set to default
#endif

	memError |= !fram_read_private(0,MEM_OFFSET, &info, sizeof(sys_info));
	memError |= !fram_read_private(0,MEM_OFFSET + sizeof(sys_info),&status,sizeof(sys_status));

	status.fram_init_cycles++;
	memError |= !fram_write_private(0,MEM_OFFSET + sizeof(sys_info),&status,sizeof(sys_status));
}

int fram_busy()
{
	return hi2c1.State != HAL_I2C_STATE_READY;
}

int fram_write(uint8_t page, uint8_t address, void* data, size_t dataLength)
{
	if(memError)
		return 0;
	return fram_write_private(page + 1, address, data, dataLength);
}

int fram_read(uint8_t page, uint8_t address, void* data, size_t dataLength)
{
	if(memError)
		return 0;
	return fram_read_private(page + 1, address, data, dataLength);
}

#if BLOCKING_MODE != 1
int fram_read_DMA(uint8_t page, uint8_t address, void* data, size_t dataLength)
{
	if(memError)
			return 0;
		return fram_read_private_DMA(page + 1, address, data, dataLength);
}
#endif

sys_status fram_getSystemStatus()
{
	return status;
}

sys_info fram_getSystemInfo()
{
	return info;
}
