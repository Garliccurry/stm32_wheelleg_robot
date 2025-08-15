#ifndef __INFO_H__
#define __INFO_H__

/**********************    INCLUDE DIRECTIVES    *************************/
#include "main.h"
#define ATOMIC_WRITE(ptr, value) \
    do {                         \
        __disable_irq();         \
        *(ptr) = (value);        \
        __enable_irq();          \
    } while (0)

#define ATOMIC_READ(ptr) ({ \
    __typeof__(*(ptr)) val; \
    __disable_irq();        \
    val = *(ptr);           \
    __enable_irq();         \
    val;                    \
})
#define WL_OK    0
#define WL_ERROR 1

#define MPU6050_I2C_DATASIZE   14
#define MPU6050_INT16_DATASIZE 7

#define AS_BUF_LEN  10
#define MPU_BUF_LEN 5
/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/

typedef struct {
    uint16_t dataL;
    uint16_t dataR;
} AsRawData_t;

typedef struct {
    int16_t data[MPU6050_INT16_DATASIZE];
} MpuRawData_t;

typedef struct
{
    float    temp;
    float    accX;
    float    accY;
    float    accZ;
    float    gyroX;
    float    gyroY;
    float    gyroZ;
    uint32_t timestamp;
} MpuData_t;

typedef struct
{
    float    angle_pre;
    float    rotat_pre;
    uint32_t angle_pre_timestamp;
    float    Gvel_angle_pre;
    float    Gvel_rotat_pre;
    uint32_t Gvel_angle_pre_timestamp;
} AsData_t;

typedef enum {
    WL_ERRINIT = 65536, // Error code for placeholder
    WL_ERR65537,        // can not get battery voltage
    WL_ERR65538,
    WL_ERR65539,
} WlErrorCode;

extern uint8_t gflag_usartrec;
extern uint8_t gflag_fatalerr;

extern float gVoltage;

/***********************    FUNCTION PROTOTYPES    ***********************/
void info_atomic_write(int *ptr, int value);
int  info_atomic_read(int *ptr);
#endif
