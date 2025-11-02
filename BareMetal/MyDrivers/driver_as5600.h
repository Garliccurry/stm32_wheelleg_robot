#ifndef __DRIVER_AS5600_H__
#define __DRIVER_AS5600_H__

/**********************    INCLUDE DIRECTIVES    ***********************/
#include "main.h"
#include "device.h"
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

typedef struct
{
    float    angle_pre;
    float    rotat_pre;
    uint32_t angle_pre_us_ts;
    float    angle_get_vel;
    float    rotat_get_vel;
    uint32_t angle_get_vel_us_ts;
    float    shaft_vel;
} AsData_t;

/***********************    FUNCTION PROTOTYPES    ***********************/
I2cDevice_t      *AS5600_GetHandle(AS5600Dir dir);
void              AS5600_Init(void);
HAL_StatusTypeDef AS5600_DmaReadData(I2cDevice_t *dev, uint8_t *asdata);
HAL_StatusTypeDef AS5600_NorReadData(I2cDevice_t *dev, uint8_t *asdata);
float             AS5600_GetAngFromRaw(uint16_t raw_data);
void              AS5600_GetVel(AsData_t *as5600);
void              AS5600_AngleUpdate(AsData_t *asdata, float angle);
// void AS5600_AngleUpdate(AsData_t *as5600, float angle);
#endif
