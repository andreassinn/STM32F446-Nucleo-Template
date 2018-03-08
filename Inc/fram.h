/*
 * fram.h
 *
 *  Created on: 08.03.2018
 *      Author: peter
 */

#ifndef FRAM_H_
#define FRAM_H_

#define BLOCKING_MODE 1

#include <inttypes.h>
#include <stddef.h>

typedef struct
{
	uint32_t fram_init_cycles;
}sys_status;

typedef struct
{
	char name[20];
	uint32_t memAvailable;
}sys_info;

void fram_init();
int fram_busy();
int fram_write(uint8_t page, uint8_t address, void* data, size_t dataLength);
int fram_read(uint8_t page, uint8_t address, void* data, size_t dataLength);

#if BLOCKING_MODE != 1
int fram_read_DMA(uint8_t page, uint8_t address, void* data, size_t dataLength);
#endif


#endif /* FRAM_H_ */
