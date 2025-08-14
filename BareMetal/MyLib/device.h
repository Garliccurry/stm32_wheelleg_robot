#ifndef __DEVICE_H__
#define __DEVICE_H__
#include "main.h"
#include "filter.h"
typedef enum {
    I2C_DEVICE_UNKNOWN = 0,
    I2C_DEVICE_MPU6050,
    I2C_DEVICE_AS5600,
    I2C_DEVICE_BOTTOM
} I2CDeviceTypeEnum;

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint8_t            dev_addr;
    uint16_t           dev_id;
    I2CDeviceTypeEnum  dev_type;

} I2cDevice_t;

HAL_StatusTypeDef Device_I2C_Register(I2cDevice_t *dev, I2C_HandleTypeDef *hi2c, uint8_t addr, I2CDeviceTypeEnum type);
#endif
