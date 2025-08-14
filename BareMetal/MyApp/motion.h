#ifndef __MOTION_H__
#define __MOTION_H__
#include "main.h"
#define MAX_FREQUENCY_TIM2    10000
#define MPU_TRIG_NUM_EARCH_AS 1

typedef enum {
    DataIdle = 0,
    DataReady,
} DateState;

typedef enum {
    BusIdle = 0,
    BusA,
    BusM,
} BusState;

void Motion_TimerGetSensor(void);
void Motion_GetSensorCallback(I2C_HandleTypeDef *hi2c);

void Motion_GetSensorData(void);
void Motion_Init(void);
#endif