#ifndef __DRIVER_AS5600_H__
#define __DRIVER_AS5600_H__

/**********************    INCLUDE DIRECTIVES    ***********************/
#include "main.h"
#include "device.h"
/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/
#define AS5600_ADDRESS 0x6C
/* AS5600 Output Registers */
#define AS5600_REGISTER_RAW_ANGLE_HIGH 0x0C
#define AS5600_REGISTER_RAW_ANGLE_LOW  0x0D
#define AS5600_TIMEOUT                 1000

#define I2C_IT_ON  1
#define I2C_IT_OFF 0

/***********************    FUNCTION PROTOTYPES    ***********************/
I2C_Device *AS5600_GetHandle(int i);
void        AS5600_Init(void);
void        AS5600_DataInit(AS5600_Data *data_l, AS5600_Data *data_r);
void        AS5600_ReadData(I2C_Device *dev, uint8_t *data);
float       AS5600_GetAng(uint16_t raw_data);
// float AS5600_GetVel(AS5600_Data *as5600);
// void AS5600_AngleUpdate(AS5600_Data *as5600, float angle);
void AS5600_ParseData(AS5600_Data *asdata, uint16_t raw_angle, float *angle, float *rotation, float *velocity);
#endif
