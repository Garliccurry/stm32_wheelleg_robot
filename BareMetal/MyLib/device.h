#ifndef __DEVICE_H__
#define __DEVICE_H__
#include "main.h"
#include "filter.h"

typedef enum
{
    I2C_DEVICE_UNKNOWN = 0,
    I2C_DEVICE_MPU6050,
    I2C_DEVICE_AS5600,
    I2C_DEVICE_BOTTOM
} I2CDeviceType;

// MPU6050�������ݽṹ
typedef struct
{
    float roll;  // �����Ĺ�ת��
    float pitch; // ������
    float yaw;   // ƫ����
} MPU6050_Data;

// AS5600�������ݽṹ
typedef struct
{
    float angle_pre;
    float angle_now;
    float rota_now;
    float rota_pre;
    uint32_t time_pre;
    LPF_TypeDef *ltf;
} AS5600_Data;

// I2C�豸ͨ�ýṹ��
typedef struct
{
    I2C_HandleTypeDef *hi2c; // I2C���������
    uint8_t dev_addr;        // �豸��ַ
    uint16_t dev_id;         // �豸Ӳ��ID
    I2CDeviceType dev_type;  // �豸���ͱ�ʶ

} I2C_Device;

HAL_StatusTypeDef Device_I2C_Register(I2C_Device *dev, I2C_HandleTypeDef *hi2c, uint8_t addr, I2CDeviceType type);
#endif
