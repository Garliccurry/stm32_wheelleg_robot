#ifndef __DRIVER_MPU6050_H
#define __DRIVER_MPU6050_H

#include <stdint.h>

#include "main.h"
#include "device.h"
//****************************************
// ����MPU6050�ڲ���ַ
//****************************************
#define MPU6050_SMPLRT_DIV   0x19 // �����ǲ����ʣ�����ֵ��0x07(125Hz)
#define MPU6050_CONFIG       0x1A // ��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define MPU6050_GYRO_CONFIG  0x1B // �������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define MPU6050_ACCEL_CONFIG 0x1C // ���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)

#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_XOUT_L 0x3C
#define MPU6050_ACCEL_YOUT_H 0x3D
#define MPU6050_ACCEL_YOUT_L 0x3E
#define MPU6050_ACCEL_ZOUT_H 0x3F
#define MPU6050_ACCEL_ZOUT_L 0x40
#define MPU6050_TEMP_OUT_H   0x41
#define MPU6050_TEMP_OUT_L   0x42
#define MPU6050_GYRO_XOUT_H  0x43
#define MPU6050_GYRO_XOUT_L  0x44
#define MPU6050_GYRO_YOUT_H  0x45
#define MPU6050_GYRO_YOUT_L  0x46
#define MPU6050_GYRO_ZOUT_H  0x47
#define MPU6050_GYRO_ZOUT_L  0x48

#define MPU6050_PWR_MGMT_1 0x6B //��Դ����������ֵ��0x00(��������)
#define MPU6050_PWR_MGMT_2 0x6C
#define MPU6050_WHO_AM_I   0x75 // IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)

#define MPU6050_ADDR    0xD2
#define MPU6050_TIMEOUT 500

/* ��������������ֵ������оƬ�̶�������Ӳ�����е����� */
#define MPU6050_X_ACCEL_OFFSET (0)
#define MPU6050_Y_ACCEL_OFFSET (0)
#define MPU6050_Z_ACCEL_OFFSET (0)
#define MPU6050_X_GYRO_OFFSET  (0)
#define MPU6050_Y_GYRO_OFFSET  (0)
#define MPU6050_Z_GYRO_OFFSET  (0)

//#define MPU6050_X_ACCEL_OFFSET	(-17)
//#define MPU6050_Y_ACCEL_OFFSET 	(13)
//#define MPU6050_Z_ACCEL_OFFSET 	(0)
//#define MPU6050_X_GYRO_OFFSET 	(16)
//#define MPU6050_Y_GYRO_OFFSET 	(-3)
//#define MPU6050_Z_GYRO_OFFSET 	(0)

#define MPU6050_QUEUE_LEN 10

struct mpu6050_data {
    int32_t angle_x;
};
I2C_Device *MPU6050_GetHandle(void);
void        MPU6050_Init(void);
int         MPU6050_GetID(void);
void        MPU6050_ReadData(uint8_t *data);
void        MPU6050_ParseData(int16_t *data, float *Ang_x, float *Ang_y);

#endif /* __DRIVER_OLED_H */
