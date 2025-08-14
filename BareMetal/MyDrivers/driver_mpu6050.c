#include "driver_mpu6050.h"
#include <math.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c3;
static I2cDevice_t        g_MPU6050;
static LPF_TypeDef       g_lpf_roll;

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

HAL_StatusTypeDef MPU6050_ReadData(uint8_t *data) // 10字节380us
{
    return MPU6050_MemReadRegister(&g_MPU6050, MPU6050_ACCEL_XOUT_H, data, MPU6050_DATASIZE);
}

void MPU6050_Init(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    Device_I2C_Register(&g_MPU6050, &hi2c3, MPU6050_ADDR, I2C_DEVICE_MPU6050);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_PWR_MGMT_1, 0x00);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_PWR_MGMT_2, 0x00);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_SMPLRT_DIV, 0x09);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_CONFIG, 0x06);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_GYRO_CONFIG, 0x08);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_ACCEL_CONFIG, 0x00);
    LPF_Init(&g_lpf_roll, 0, 0.2);
    if (status == HAL_OK) {
        LOG_INFO("mpu6050 initialization successful!");
    } else {
        LOG_INFO("mpu6050 initialization failed!");
    }
    HAL_Delay(10);
}

void MPU6050_GetData(MpuData_t *mdata, uint8_t *rawdata)
{
    int16_t rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ;
    rawAccX = rawdata[0] << 8 | rawdata[1];
    rawAccY = rawdata[2] << 8 | rawdata[3];
    rawAccZ = rawdata[4] << 8 | rawdata[5];
    rawTemp = rawdata[6] << 8 | rawdata[7];
    rawGyroX = rawdata[8] << 8 | rawdata[9];
    rawGyroY = rawdata[10] << 8 | rawdata[11];
    rawGyroZ = rawdata[12] << 8 | rawdata[13];

    mdata->temp = (rawTemp + 12412.0) / 340.0;
    mdata->accX = ((float)rawAccX) / 16384.0;
    mdata->accY = ((float)rawAccY) / 16384.0;
    mdata->accZ = ((float)rawAccZ) / 16384.0;
    mdata->gyroX = ((float)rawGyroX) / 65.5;
    mdata->gyroY = ((float)rawGyroY) / 65.5;
    mdata->gyroZ = ((float)rawGyroZ) / 65.5;
}

void MPU6050_ParseData(int16_t *data, float *Ang_roll, float *Ang_pitch)
{
    float roll, pitch;
    roll = atan2((double)(data[0] + MPU6050_X_ACCEL_OFFSET), (double)(data[2] + MPU6050_Z_ACCEL_OFFSET)) * 57.29577;
    pitch = atan2((double)(data[1] + MPU6050_Y_ACCEL_OFFSET), (double)(data[2] + MPU6050_Z_ACCEL_OFFSET)) * 57.29577;
    *Ang_roll = LowPassFilter(&g_lpf_roll, roll);
    *Ang_pitch = pitch;
}
