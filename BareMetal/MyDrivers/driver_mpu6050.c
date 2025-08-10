#include "driver_mpu6050.h"
#include "log.h"
#include <math.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c3;
static I2C_Device        g_MPU6050;
static LPF_TypeDef       g_lpf_roll;

I2C_Device *MPU6050_GetHandle(void)
{
    return &g_MPU6050;
}

static HAL_StatusTypeDef MPU6050_WriteRegister(I2C_Device *dev, uint8_t reg, uint8_t data)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = reg;
    tmpbuf[1] = data;

    return HAL_I2C_Master_Transmit(dev->hi2c, dev->dev_addr, tmpbuf, 2, MPU6050_TIMEOUT);
}

HAL_StatusTypeDef MPU6050_ReadRegister(I2C_Device *dev, uint8_t reg, uint8_t *pdata, uint16_t size)
{
    return HAL_I2C_Mem_Read_DMA(dev->hi2c, dev->dev_addr,
                                reg, I2C_MEMADD_SIZE_8BIT,
                                pdata, size);
}

int MPU6050_GetID(void)
{
    uint8_t id;
    if (HAL_OK == MPU6050_ReadRegister(&g_MPU6050, MPU6050_WHO_AM_I, &id, 1))
        return id;
    else
        return -1;
}

void MPU6050_ReadData(uint8_t *data) // 10字节380us
{
    HAL_StatusTypeDef status = HAL_OK;
    if (data) {
        status |= MPU6050_ReadRegister(&g_MPU6050, MPU6050_ACCEL_XOUT_H, &data[0], 6);
        status |= MPU6050_ReadRegister(&g_MPU6050, MPU6050_GYRO_YOUT_H, &data[6], 4);
    }

    if (status != HAL_OK) {
        printf("IMU %d\r\n", status);
    }
}

void MPU6050_Init(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    Device_I2C_Register(&g_MPU6050, &hi2c3, MPU6050_ADDR, I2C_DEVICE_MPU6050);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_PWR_MGMT_1, 0x00);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_PWR_MGMT_2, 0x00);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_SMPLRT_DIV, 0x09);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_CONFIG, 0x06);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_GYRO_CONFIG, 0x18);
    status |= MPU6050_WriteRegister(&g_MPU6050, MPU6050_ACCEL_CONFIG, 0x18);
    LPF_Init(&g_lpf_roll, 0, 0.2);
    if (status == HAL_OK) {
        LOG_INFO("mpu6050 initialization successful!");
    } else {
        LOG_INFO("mpu6050 initialization failed!");
    }
    HAL_Delay(10);
}

void MPU6050_ParseData(int16_t *data, float *Ang_roll, float *Ang_pitch)
{
    float roll, pitch;
    roll = atan2((double)(data[0] + MPU6050_X_ACCEL_OFFSET), (double)(data[2] + MPU6050_Z_ACCEL_OFFSET)) * 57.29577;
    pitch = atan2((double)(data[1] + MPU6050_Y_ACCEL_OFFSET), (double)(data[2] + MPU6050_Z_ACCEL_OFFSET)) * 57.29577;
    *Ang_roll = LowPassFilter(&g_lpf_roll, roll);
    *Ang_pitch = pitch;
}
