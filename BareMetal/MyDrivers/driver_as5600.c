#include "driver_as5600.h"
#include <math.h>
#include <stdlib.h>
#include "i2c.h"
#include "usart.h"
#include "foc.h"
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c3;
static I2C_Device AS5600_L, AS5600_R;
static LPF_TypeDef lpf_l, lpf_r;

I2C_Device *AS5600_GetHandle(int i)
{
    if (i == 1)
        return &AS5600_L;
    if (i == 2)
        return &AS5600_R;
    else
        return NULL;
}

void AS5600_Init(void)
{
    Device_I2C_Register(&AS5600_L, &hi2c1, AS5600_ADDRESS, I2C_DEVICE_AS5600);
    Device_I2C_Register(&AS5600_R, &hi2c3, AS5600_ADDRESS, I2C_DEVICE_AS5600);
    printf("as5600 init succ\r\n");
}

void AS5600_DataInit(AS5600_Data *data_l, AS5600_Data *data_r)
{
    data_l->ltf = &lpf_l;
    data_r->ltf = &lpf_r;
    LPF_Init(&lpf_l, 0.5);
    LPF_Init(&lpf_r, 0.5);
}

void AS5600_ReadData(I2C_Device *dev, uint8_t *asdata)
{
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_I2C_Mem_Read(dev->hi2c, dev->dev_addr,
                              AS5600_REGISTER_RAW_ANGLE_HIGH,
                              I2C_MEMADD_SIZE_8BIT, asdata, 2,
                              AS5600_TIMEOUT);
    if (status != HAL_OK)
    {
        printf("AS %d\r\n", status);
    }
}

float AS5600_GetAng(uint16_t raw_data)
{
    return (float)raw_data * _PI / 2048.f; // ���ػ���ֵ0-2PI
}

static void AS5600_AngleUpdate(AS5600_Data *asdata, float angle, float *rot)
{
    float angle_d;

    angle_d = angle - asdata->angle_now;
    if (fabs(angle_d) > (0.8f * 6.28318530718f))
    {
        asdata->rota_now += (angle_d > 0) ? -1 : 1;
        *rot = asdata->rota_now;
    }
    asdata->angle_now = angle;
}

float AS5600_GetVel(AS5600_Data *asdata)
{
    float Ts = 0.01;

    float vel = ((double)(asdata->rota_now - asdata->rota_pre) * _2PI + (asdata->angle_now - asdata->angle_pre)) / Ts;
    asdata->rota_pre = asdata->rota_now;
    asdata->angle_pre = asdata->angle_now;
    //    if(vel > 500.0f || vel < -500.0f) vel = 0.0f;
    return vel;
}

void AS5600_ParseData(AS5600_Data *asdata, uint16_t raw_angle, float *angle, float *rotation, float *velocity)
{
    *angle = AS5600_GetAng(raw_angle);
    float vel = AS5600_GetVel(asdata);
    *velocity = LowPassFilter(asdata->ltf, vel);
    AS5600_AngleUpdate(asdata, *angle, rotation);
    //    printf("%f, %f\n",vel,*velocity);
}
