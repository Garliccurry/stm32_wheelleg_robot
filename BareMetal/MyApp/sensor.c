#include <stdlib.h>
#include "log.h"
#include "tim.h"
#include "sensor.h"
#include "driver_as5600.h"
#include "driver_mpu6050.h"
static uint32_t g_Prescale = 5;

static I2cDevice_t *g_I2CASL;
static I2cDevice_t *g_I2CASR;
static I2cDevice_t *g_I2CMPU;

static uint8_t g_ASRawDataL[AS5600_I2C_DATASIZE];
static uint8_t g_ASRawDataR[AS5600_I2C_DATASIZE];
static uint8_t g_MPURawData[MPU6050_I2C_DATASIZE];

static volatile uint8_t g_I2CBus = BusIdle;

static AsRawData_t  *g_AsRawDataBuf = NULL;
static MpuRawData_t *g_MpuRawDataBuf = NULL;

static CirBuf_t g_CirAsRawBuff = {0};
static CirBuf_t g_CirMpuRawBuff = {0};

void Sensor_SetSensorGetDataFre(uint32_t frequency)
{
    if (frequency > MAX_FREQUENCY_TIM2) {
        frequency = MAX_FREQUENCY_TIM2;
    }
    g_Prescale = MAX_FREQUENCY_TIM2 / frequency;
    LOG_INFO("The update frequency of the sensor is %dhz", frequency);
}

static void Sensor_StartGetAS5600(void)
{
    ATOMIC_WRITE(&g_I2CBus, (uint8_t)BusA);
    uint32_t status = HAL_OK;
    status |= AS5600_ReadData(g_I2CASR, g_ASRawDataR);
    status |= AS5600_ReadData(g_I2CASL, g_ASRawDataL);
    if (status != HAL_OK) {
        g_I2cErrorCount++;
        return;
    }
    g_flagI2cError = WLR_Off;
}

static void Sensor_StartGetMPU6050(void)
{
    ATOMIC_WRITE(&g_I2CBus, (uint8_t)BusM);
    uint32_t status = HAL_OK;
    status = MPU6050_MemReadData(g_MPURawData);
    if (status != HAL_OK) {
        g_I2cErrorCount++;
        return;
    }
    g_flagI2cError = WLR_Off;
}
void Sensor_TimerGetSensor(void) // 定时器周期回调
{
    static uint32_t TIM2base_cnt = 0;
    static uint8_t  change_flag = 0;
    if (TIM2base_cnt == g_Prescale) {
        if (change_flag == 0) {
            Sensor_StartGetAS5600();
            change_flag = 1;
        } else {
            Sensor_StartGetMPU6050();
            change_flag = 0;
        }
        TIM2base_cnt = 0;
    }
    TIM2base_cnt++;
}

void Sensor_GetSensorCallback(I2C_HandleTypeDef *hi2c) // i2c mem中断回调
{
    static uint8_t AsCount = 0;
    static uint8_t BusStatus = 0;
    BusStatus = ATOMIC_READ(&g_I2CBus);
    if (BusStatus == BusA) {
        AsCount++;
        if (AsCount == 2) {
            CirBuf_AsDataWrite(&g_CirAsRawBuff, g_ASRawDataL[0] << 8 | g_ASRawDataL[1], g_ASRawDataR[0] << 8 | g_ASRawDataR[1]);
            AsCount = 0;
        }
    } else if (BusStatus == BusM) {
        MpuRawData_t rawdata;
        for (uint8_t idx = 0; idx < MPU6050_INT16_DATASIZE; idx++) {
            rawdata.data[idx] = (int16_t)(g_MPURawData[2 * idx] << 8 | g_MPURawData[2 * idx + 1]);
        }
        CirBuf_MpuDataWrite(&g_CirMpuRawBuff, &rawdata);
        AsCount = 0;
    }
}

void Sensor_GetFocData(void)
{
    static uint16_t shaft_raw_angle_L, shaft_raw_angle_R;
    static float    shaft_angle_L, shaft_angle_R;
    if (CirBuf_AsDataRead(&g_CirAsRawBuff, &shaft_raw_angle_L, &shaft_raw_angle_R) == WLR_OK) {
        shaft_angle_L = AS5600_GetAngFromRaw(shaft_raw_angle_L);
        shaft_angle_R = AS5600_GetAngFromRaw(shaft_raw_angle_R);

        AS5600_AngleUpdate(&g_ASdataL, shaft_angle_L);
        AS5600_AngleUpdate(&g_ASdataR, shaft_angle_R);

        AS5600_GetVel(&g_ASdataL);
        AS5600_GetVel(&g_ASdataR);
        // LOG_DEBUG("%f, %f", shaft_angle_L, shaft_angle_R);
        g_flagFocDate = WLR_Act;
    }
}

void Sensor_GetMpuData(void)
{
    static MpuRawData_t rawdata;
    if (CirBuf_MpuDataRead(&g_CirMpuRawBuff, &rawdata) == WLR_OK) {
        MPU6050_GetData(&g_MPUdata, rawdata.data);
        // LOG_DEBUG("%f,%f,%f", g_MPUdata.accX, g_MPUdata.accY, g_MPUdata.accZ);
        g_flagMpuDate = WLR_Act;
    }
}

void Sensor_Init(void)
{
    g_AsRawDataBuf = (AsRawData_t *)malloc(sizeof(AsRawData_t) * AS_BUF_LEN);
    g_MpuRawDataBuf = (MpuRawData_t *)malloc(sizeof(MpuRawData_t) * MPU_BUF_LEN);
    CirBuf_AsDataInit(&g_CirAsRawBuff, AS_BUF_LEN, g_AsRawDataBuf);
    CirBuf_MpuDataInit(&g_CirMpuRawBuff, MPU_BUF_LEN, g_MpuRawDataBuf);

    g_I2CASL = AS5600_GetHandle(AS5600Left);
    g_I2CASR = AS5600_GetHandle(AS5600Right);
    g_I2CMPU = MPU6050_GetHandle();

    AS5600_Init();
    MPU6050_Init();

    HAL_TIM_Base_Start_IT(&htim2);
}
