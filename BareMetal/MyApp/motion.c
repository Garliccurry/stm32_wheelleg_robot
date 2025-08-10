#include "motion.h"
#include "log.h"
#include "tim.h"
#include "driver_as5600.h"
#include "driver_mpu6050.h"
static uint32_t     gPrescale = 100;
static SensorStruct sensor = {
    .BusStatus = SensorIdle,
    .StateM = DataIdle,
    .StateL = DataIdle,
    .StateR = DataIdle};

static I2C_Device *gI2C_AS_L;
static I2C_Device *gI2C_AS_R;

static uint8_t ASRawBuf_L[AS5600_I2C_DATASIZE];
static uint8_t ASRawBuf_R[AS5600_I2C_DATASIZE];

static AS5600_Data ASdata_L;
static AS5600_Data ASdata_R;

static void Motion_GetAS5600(void);

void Motion_SetSensorGetFre(uint32_t frequency)
{
    if (frequency > MAX_FREQUENCY_TIM2) {
        frequency = MAX_FREQUENCY_TIM2;
    }
    gPrescale = MAX_FREQUENCY_TIM2 / frequency;
    LOG_INFO("The update frequency of the sensor is %dhz", frequency);
}

void Motion_GetSensor(void)
{
    static uint32_t tim_cnt = 0;
    if (tim_cnt == gPrescale) {
        // LOG_DEBUG("tim2");
        Motion_GetAS5600();
        tim_cnt = 0;
    }
    tim_cnt++;
}

static void Motion_GetAS5600(void)
{
    sensor.BusStatus = SensorAsRun;
    sensor.StateL = DataPending;
    sensor.StateR = DataPending;
    AS5600_ReadData(gI2C_AS_L, ASRawBuf_L);
    AS5600_ReadData(gI2C_AS_R, ASRawBuf_R);
}

void Motion_SensorUpdateCallback(I2C_HandleTypeDef *hi2c)
{
    if (sensor.BusStatus == SensorAsRun) {
        if (hi2c == gI2C_AS_L->hi2c) {
            sensor.StateL = DataReady;
        } else if (hi2c == gI2C_AS_R->hi2c) {
            sensor.StateR = DataReady;
        }
        sensor.BusStatus = SensorIdle;
    } else if (sensor.BusStatus == SensorMpuRun) {
        sensor.StateM = DataReady;
        sensor.BusStatus = SensorIdle;
        return;
    } else {
        LOG_ERROR("error i2c call back, Bus state: %d", sensor.BusStatus);
    }
}

void Motion_GetSensorData(void)
{
    if (sensor.StateL && sensor.StateR) {
        // LOG_DEBUG("AFTER");
        float shaft_angle_L = AS5600_GetAng(ASRawBuf_L[0] << 8 | ASRawBuf_L[1]);
        float shaft_angle_R = AS5600_GetAng(ASRawBuf_R[0] << 8 | ASRawBuf_R[1]);
        AS5600_AngleUpdate(&ASdata_L, shaft_angle_L);
        AS5600_AngleUpdate(&ASdata_R, shaft_angle_R);
        LOG_DEBUG("%f, %f", shaft_angle_L, shaft_angle_R);
        sensor.StateL = DataIdle;
        sensor.StateR = DataIdle;
    } else if (sensor.StateM == DataReady) {
        sensor.StateM = DataIdle;
    }
}

void Motion_Init(void)
{
    MPU6050_Init();
    AS5600_Init();

    gI2C_AS_L = AS5600_GetHandle(AS5600Left);
    gI2C_AS_R = AS5600_GetHandle(AS5600Right);

    HAL_TIM_Base_Start_IT(&htim2);
}