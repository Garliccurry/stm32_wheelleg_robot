#include <stdlib.h>
#include "log.h"
#include "tim.h"
#include "motion.h"
#include "cirbuf.h"
#include "driver_as5600.h"
#include "driver_mpu6050.h"
static uint32_t gPrescale = 5;

static I2cDevice_t *gI2C_ASL;
static I2cDevice_t *gI2C_ASR;
static I2cDevice_t *gI2C_MPU;

static uint8_t gASRawDataL[AS5600_I2C_DATASIZE];
static uint8_t gASRawDataR[AS5600_I2C_DATASIZE];
static uint8_t gMPURawData[MPU6050_DATASIZE];

static AsData_t  gASdata_L;
static AsData_t  gASdata_R;
static MpuData_t gMPUdata;

static uint8_t gStateM = DataIdle;

static volatile uint8_t gBus = BusIdle;

static AsRawDataBuf_t *gAsRawDataBuf = NULL;
static CirBuf_t        gCirAsRawBuff = {0};

void Motion_SetSensorGetFre(uint32_t frequency)
{
    if (frequency > MAX_FREQUENCY_TIM2) {
        frequency = MAX_FREQUENCY_TIM2;
    }
    gPrescale = MAX_FREQUENCY_TIM2 / frequency;
    LOG_INFO("The update frequency of the sensor is %dhz", frequency);
}

static void Motion_GetAS5600(void)
{
    ATOMIC_WRITE(&gBus, (uint8_t)BusA);
    uint32_t status = HAL_OK;
    status |= AS5600_ReadData(gI2C_ASR, gASRawDataR);
    status |= AS5600_ReadData(gI2C_ASL, gASRawDataL);
    if (status != HAL_OK) {
        LOG_ERROR("AS I2C STATUS: %d", status);
    }
}

static void Motion_GetMPU6050(void)
{
    ATOMIC_WRITE(&gBus, (uint8_t)BusM);
    uint32_t status = HAL_OK;
    status = MPU6050_ReadData(gMPURawData);
    if (status != HAL_OK) {
        LOG_ERROR("MPU I2C STATUS: %d", status);
    }
}
void Motion_GetSensor(void) // 定时器周期回调
{
    static uint32_t TIM2base_cnt = 0;
    static uint8_t  change_flag = 0;
    if (TIM2base_cnt == gPrescale) {
        if (change_flag == 0) {
            Motion_GetAS5600();
            change_flag = 1;
        } else {
            Motion_GetMPU6050();
            change_flag = 0;
        }
        TIM2base_cnt = 0;
    }
    TIM2base_cnt++;
}
void Motion_GetSensorCallback(I2C_HandleTypeDef *hi2c) // i2c mem中断回调
{
    static uint8_t AsCount = 0;
    static uint8_t BusStatus = 0;
    BusStatus = ATOMIC_READ(&gBus);
    if (BusStatus == BusA) {
        AsCount++;
        if (AsCount == 2) {
            CirBuf_AsDataWrite(&gCirAsRawBuff, gASRawDataL[0] << 8 | gASRawDataL[1], gASRawDataR[0] << 8 | gASRawDataR[1]);
            AsCount = 0;
        }
    } else if (BusStatus == BusM) {
        gStateM = DataReady;
        AsCount = 0;
    }
}

void Motion_GetSensorData(void)
{
    static uint16_t shaft_raw_angle_L, shaft_raw_angle_R;
    static float    shaft_angle_L, shaft_angle_R;
    if (CirBuf_AsDataRead(&gCirAsRawBuff, &shaft_raw_angle_L, &shaft_raw_angle_R) == WL_OK) {
        shaft_angle_L = AS5600_GetAng(shaft_raw_angle_L);
        shaft_angle_R = AS5600_GetAng(shaft_raw_angle_R);

        AS5600_AngleUpdate(&gASdata_L, shaft_angle_L);
        AS5600_AngleUpdate(&gASdata_R, shaft_angle_R);
        LOG_DEBUG("%f, %f", shaft_angle_L, shaft_angle_R);
        // LOG_DEBUG("1");
    }
    if (gStateM == DataReady) {
        MPU6050_GetData(&gMPUdata, gMPURawData);
        LOG_DEBUG("%f", gMPUdata.accX);
        gStateM = DataIdle;
    }
}

void Motion_Init(void)
{
    MPU6050_Init();
    AS5600_Init();

    gAsRawDataBuf = (AsRawDataBuf_t *)malloc(sizeof(AsRawDataBuf_t) * AS_BUF_LEN);
    CirBuf_AsDataInit(&gCirAsRawBuff, AS_BUF_LEN, gAsRawDataBuf);

    gI2C_ASL = AS5600_GetHandle(AS5600Left);
    gI2C_ASR = AS5600_GetHandle(AS5600Right);
    gI2C_MPU = MPU6050_GetHandle();

    HAL_TIM_Base_Start_IT(&htim2);
}