#include "device.h"

static uint8_t g_id = 0;

HAL_StatusTypeDef Device_I2C_Register(I2cDevice_t *dev, I2C_HandleTypeDef *hi2c, uint8_t addr, I2CDeviceTypeEnum type)
{
    if (!(dev && hi2c) || (dev->hi2c) || (dev->dev_addr) || (dev->dev_type)) {
        return HAL_ERROR;
    }
    dev->hi2c = hi2c;
    dev->dev_addr = addr;
    dev->dev_type = type;
    dev->dev_id = g_id;
    g_id++;
    return HAL_OK;
}
