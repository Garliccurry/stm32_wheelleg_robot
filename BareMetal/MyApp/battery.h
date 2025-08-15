#ifndef __BATTERY_H__
#define __BATTERY_H__

#include "stm32f4xx_hal.h"

void     Battery_Init(void);
uint32_t Battery_GetData(void);
uint8_t  Battery_SetTogFre(void);
#endif
