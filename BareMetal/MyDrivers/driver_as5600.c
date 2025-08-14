#include "driver_as5600.h"
#include <math.h>
#include <stdlib.h>
#include "i2c.h"
#include "log.h"
#include "foc.h"
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c3;

static I2cDevice_t gI2C_AS_L, gI2C_AS_R;
static LPF_TypeDef gLPF_L, gLPF_R;

I2cDevice_t *AS5600_GetHandle(int dir)
{
    if (dir == AS5600Left)
        return &gI2C_AS_L;
    if (dir == AS5600Right)
        return &gI2C_AS_R;
    else
        return NULL;
}

void AS5600_Init(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    status |= Device_I2C_Register(&gI2C_AS_L, &hi2c1, AS5600_ADDRESS, I2C_DEVICE_AS5600);
    status |= Device_I2C_Register(&gI2C_AS_R, &hi2c3, AS5600_ADDRESS, I2C_DEVICE_AS5600);
    if (status == HAL_OK) {
        LOG_INFO("as5600 initialization successful!");
    } else {
        LOG_INFO("as5600 initialization failed!");
    }
    HAL_Delay(10);
}

HAL_StatusTypeDef AS5600_ReadData(I2cDevice_t *dev, uint8_t *asdata) // 2字节100us
{
    return HAL_I2C_Mem_Read_DMA(dev->hi2c, dev->dev_addr,
                                AS5600_REGISTER_RAW_ANGLE_HIGH,
                                I2C_MEMADD_SIZE_8BIT, asdata, AS5600_I2C_DATASIZE);
}

float AS5600_GetAng(uint16_t raw_data)
{
    return (float)raw_data * _PI / 2048.f;
}

float AS5600_GetVel(AsData_t *asdata)
{
    float Ts = 0.01;

    float vel = ((double)(asdata->rotat_pre - asdata->Gvel_rotat_pre) * _2PI + (asdata->angle_pre - asdata->Gvel_angle_pre)) / Ts;

    asdata->Gvel_angle_pre = asdata->angle_pre;
    asdata->Gvel_rotat_pre = asdata->Gvel_rotat_pre;
    asdata->Gvel_angle_pre = asdata->angle_pre;
    //    if(vel > 500.0f || vel < -500.0f) vel = 0.0f;
    return vel;
}
void AS5600_AngleUpdate(AsData_t *asdata, float angle)
{
    float angle_d;

    angle_d = angle - asdata->angle_pre;
    if (fabs(angle_d) > (0.8f * 6.28318530718f)) {
        asdata->rotat_pre += (angle_d > 0) ? -1 : 1;
    }
    asdata->angle_pre = angle;
}