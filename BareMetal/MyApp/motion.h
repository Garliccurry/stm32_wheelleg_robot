#ifndef __MOTION_H__
#define __MOTION_H__
#include "main.h"
#define MAX_FREQUENCY_TIM2 10000

typedef enum {
    SensorIdle = 0,
    SensorAsRun,
    SensorMpuRun,
    SensorDevStateBottom
} SensorState;

typedef enum {
    DataIdle = 0,
    DataReady,
    DataPending,
    DataBottom
} DateState;

typedef struct SensorStruct {
    int BusStatus;
    int StateM;
    int StateL;
    int StateR;
} SensorStruct;

void Motion_GetSensor(void);
void Motion_SensorUpdateCallback(I2C_HandleTypeDef *hi2c);

void Motion_GetSensorData(void);
void Motion_Init(void);
#endif