#include "driver_mpu6050.h"
#include <math.h>
#include <stdio.h>
#include "sensor.h"

extern I2C_HandleTypeDef hi2c3;
static I2cDevice_t       g_MPU6050;

I2cDevice_t *MPU6050_GetHandle(void)
{
    return &g_MPU6050;
}

static HAL_StatusTypeDef MPU6050_WriteRegister(I2cDevice_t *dev, uint8_t reg, uint8_t data)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = reg;
    tmpbuf[1] = data;

    return HAL_I2C_Master_Transmit(dev->hi2c, dev->dev_addr, tmpbuf, 2, MPU6050_TIMEOUT);
}

static HAL_StatusTypeDef MPU6050_NorReadRegister(I2cDevice_t *dev, uint8_t reg, uint8_t *pdata, uint16_t size)
{
    return HAL_I2C_Mem_Read(dev->hi2c, dev->dev_addr,
                            reg, I2C_MEMADD_SIZE_8BIT,
                            pdata, size, 1000);
}

static HAL_StatusTypeDef MPU6050_MemReadRegister(I2cDevice_t *dev, uint8_t reg, uint8_t *pdata, uint16_t size)
{
    return HAL_I2C_Mem_Read_DMA(dev->hi2c, dev->dev_addr,
                                reg, I2C_MEMADD_SIZE_8BIT,
                                pdata, size);
}

int MPU6050_GetID(void)
{
    uint8_t id;
    if (HAL_OK == MPU6050_NorReadRegister(&g_MPU6050, MPU6050_WHO_AM_I, &id, 1))
        return id;
    else
        return -1;
}

HAL_StatusTypeDef MPU6050_MemReadData(uint8_t *data) // 10字节380us
{
    return MPU6050_MemReadRegister(&g_MPU6050, MPU6050_ACCEL_XOUT_H, data, MPU6050_I2C_DATASIZE);
}

HAL_StatusTypeDef MPU6050_NorReadData(uint8_t *data) // 10字节380us
{
    return MPU6050_NorReadRegister(&g_MPU6050, MPU6050_ACCEL_XOUT_H, data, MPU6050_I2C_DATASIZE);
}

void MPU6050_GetData(MpuData_t *mdata, int16_t *rawdata)
{
    float accX, accY, accZ;
    accX = ((float)rawdata[0]) / 16384.0;
    accY = ((float)rawdata[1]) / 16384.0;
    accZ = ((float)rawdata[2]) / 16384.0;

    mdata->gyroX = ((float)rawdata[4]) / 65.5;
    mdata->gyroY = ((float)rawdata[5]) / 65.5;
    mdata->gyroZ = ((float)rawdata[6]) / 65.5;

    mdata->temp = ((float)rawdata[3] + 12412.0) / 340.0;

    mdata->angleAccX = atan2(accY, accZ + fabs(accX)) * 360 / 2.0 / PI;
    mdata->angleAccY = atan2(accX, accZ + fabs(accY)) * 360 / -2.0 / PI;

    mdata->gyroX -= mdata->gyroXoffset;
    mdata->gyroY -= mdata->gyroYoffset;
    mdata->gyroZ -= mdata->gyroZoffset;
    LOG_DEBUG("%f,%f,%f", mdata->gyroX, mdata->gyroY, mdata->gyroZ);

    float interval = (Info_GetUsTick() - mdata->us_ts) * 1e-6f;

    mdata->angleGyroX += mdata->gyroX * interval;
    mdata->angleGyroY += mdata->gyroY * interval;
    mdata->angleGyroZ += mdata->gyroZ * interval;

    mdata->angleX = (mdata->gyroCoef * (mdata->angleX + mdata->gyroX * interval)) + (mdata->accCoef * mdata->angleAccX);
    mdata->angleY = (mdata->gyroCoef * (mdata->angleY + mdata->gyroY * interval)) + (mdata->accCoef * mdata->angleAccY);
    mdata->angleZ = mdata->angleGyroZ;

    mdata->us_ts = Info_GetUsTick();
}

static uint32_t MPU6050_GyroOffsetCalibrate(float *gyroX, float *gyroY, float *gyroZ, int init_cnt)
{
    uint32_t ret = WLR_OK;
    uint8_t  rawdata[MPU6050_I2C_DATASIZE] = {0};
    float    X = 0;
    float    Y = 0;
    float    Z = 0;
    for (uint32_t i = 0; i < init_cnt; i++) {
        ret |= MPU6050_NorReadData(rawdata);
        BREAK_IF(ret != WLR_OK, WLR_ERR65541);

        int16_t datax = (int16_t)(rawdata[8] << 8 | rawdata[9]);
        int16_t datay = (int16_t)(rawdata[10] << 8 | rawdata[11]);
        int16_t dataz = (int16_t)(rawdata[12] << 8 | rawdata[13]);
        // LOG_DEBUG("cnt:%d, x:%f, y:%f, z:%f", i, datax / 65.5, datay / 65.5, dataz / 65.5);

        X += ((float)datax) / 65.5;
        Y += ((float)datay) / 65.5;
        Z += ((float)dataz) / 65.5;
        HAL_Delay(1);
    }
    *gyroX = X / (float)init_cnt;
    *gyroY = Y / (float)init_cnt;
    *gyroZ = Z / (float)init_cnt;
    return ret;
}

void MPU6050_Init(void)
{
    uint32_t ret = WLR_OK;
    Device_I2C_Register(&g_MPU6050, &hi2c3, MPU6050_ADDR, I2C_DEVICE_MPU6050);
    ret |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_PWR_MGMT_1, 0x00);
    ret |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_PWR_MGMT_2, 0x00);
    ret |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_SMPLRT_DIV, 0x09);
    ret |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_CONFIG, 0x06);
    ret |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_GYRO_CONFIG, 0x08);
    ret |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_ACCEL_CONFIG, 0x00);

    LOG_INFO("start Calibrating mpu6050 Gyro offset ");
    float gyroX = 0, gyroY = 0, gyroZ = 0;
    HAL_Delay(10);
    ret |= MPU6050_GyroOffsetCalibrate(&gyroX, &gyroY, &gyroZ, MPU6050_GYRO_INIT_COUNT);
    g_MPUdata.gyroXoffset = gyroX;
    g_MPUdata.gyroYoffset = gyroY;
    g_MPUdata.gyroZoffset = gyroZ;
    LOG_INFO("mpu6050 Gyro offset X:%f, Y:%f, Z:%f", g_MPUdata.gyroXoffset, g_MPUdata.gyroYoffset, g_MPUdata.gyroZoffset);
    if (ret == WLR_OK) {
        LOG_INFO("mpu6050 initialization successful!");
    } else {
        LOG_INFO("mpu6050 initialization failed, ret:%d", ret);
    }
}