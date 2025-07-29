#ifndef __DEVICE_H__
#define __DEVICE_H__
#include "main.h"
#include "filter.h"


typedef enum {
    I2C_DEVICE_UNKNOWN = 0,
    I2C_DEVICE_MPU6050,
    I2C_DEVICE_AS5600,
} I2CDeviceType;


// MPU6050特有数据结构
typedef struct {
    float roll;         // 计算后的滚转角
    float pitch;        // 俯仰角
    float yaw;          // 偏航角
} MPU6050_Data;

// AS5600特有数据结构
typedef struct {
    float angle_pre;
    float angle_now;
    float rota_now;
    float rota_pre;
    uint32_t time_pre;
    LPF_TypeDef* ltf;
} AS5600_Data;

// I2C设备通用结构体
typedef struct {
    I2C_HandleTypeDef *hi2c;        // I2C控制器句柄
    uint8_t            dev_addr;    // 设备地址
    uint16_t           dev_id;      // 设备硬件ID
    I2CDeviceType      dev_type;    // 设备类型标识
    
} I2C_Device;

HAL_StatusTypeDef xI2CDevice_Register(I2C_Device *dev, I2C_HandleTypeDef *hi2c, uint8_t addr, I2CDeviceType type);
#endif
