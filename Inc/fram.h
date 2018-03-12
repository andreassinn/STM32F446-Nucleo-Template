/*
 * fram.h
 *
 *  Created on: 08.03.2018
 *      Author: peter
 */

#ifndef FRAM_H_
#define FRAM_H_

#define BLOCKING_MODE 0

#include <inttypes.h>
#include <stddef.h>

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

void fram_init();
int fram_busy();
int fram_write(uint8_t page, uint16_t address, void* data, size_t dataLength);
int fram_read(uint8_t page, uint16_t address, void* data, size_t dataLength);
int fram_read_DMA(uint8_t page, uint16_t address, void* data, size_t dataLength);

fram_sys* fram_getSystemStatus();

#endif /* FRAM_H_ */
