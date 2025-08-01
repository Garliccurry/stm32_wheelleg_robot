#include "device.h"

static uint8_t g_id = 0;


HAL_StatusTypeDef xI2CDevice_Register(I2C_Device *dev, I2C_HandleTypeDef *hi2c, uint8_t addr, I2CDeviceType type)
{
    if (!(dev && hi2c)||(dev->hi2c)||(dev->dev_addr)||(dev->dev_type)) {
        return HAL_ERROR;
    }
    dev->hi2c = hi2c;
    dev->dev_addr = addr;
    dev->dev_type = type;
    dev->dev_id = g_id;
    g_id++;
    return HAL_OK;
}

