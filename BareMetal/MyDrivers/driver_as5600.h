#ifndef __DRIVER_AS5600_H__
#define __DRIVER_AS5600_H__

/**********************    INCLUDE DIRECTIVES    ***********************/
#include "main.h"
#include "device.h"
#include "info.h"
/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/
#define AS5600_I2C_DATASIZE 2
#define AS5600_ADDRESS      0x6C
/* AS5600 Output Registers */
#define AS5600_REGISTER_RAW_ANGLE_HIGH 0x0C
#define AS5600_REGISTER_RAW_ANGLE_LOW  0x0D
#define AS5600_TIMEOUT                 1000

#define I2C_IT_ON  1
#define I2C_IT_OFF 0

typedef enum {
    AS5600DirInit = 0,
    AS5600Left,
    AS5600Right,
    AS5600DirBottom
} AS5600Dir;
/***********************    FUNCTION PROTOTYPES    ***********************/
I2cDevice_t      *AS5600_GetHandle(AS5600Dir dir);
void              AS5600_Init(void);
HAL_StatusTypeDef AS5600_ReadData(I2cDevice_t *dev, uint8_t *data);
float             AS5600_GetAng(uint16_t raw_data);
float             AS5600_GetVel(AsData_t *as5600);
void              AS5600_AngleUpdate(AsData_t *asdata, float angle);
// void AS5600_AngleUpdate(AsData_t *as5600, float angle);
#endif
