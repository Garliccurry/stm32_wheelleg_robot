#ifndef __BATTERY_H__
#define __BATTERY_H__

#include "stm32f4xx_hal.h"

void Battery_Init(void);
void Battery_TimerCallback(void);
#endif
