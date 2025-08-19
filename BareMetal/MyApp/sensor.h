#ifndef __SENSOR_H__
#define __SENSOR_H__
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

void Sensor_TimerGetSensor(void);
void Sensor_GetSensorCallback(I2C_HandleTypeDef *hi2c);

void Sensor_GetFocData(void);
void Sensor_GetMpuData(void);

void Sensor_Init(void);
#endif