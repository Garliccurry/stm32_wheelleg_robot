#ifndef __BATTERY_H
#define __BATTERY_H

#include "stm32f4xx_hal.h"

void vBattery_Init(void);
void vBattery_TimerCallback(void);
#endif
