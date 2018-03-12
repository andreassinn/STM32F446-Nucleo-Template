#include "sys.h"
#include <stdio.h>
#include <string.h>

static uint8_t resetFlag;

void sysStartup()
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
