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
#define AS5600_TEST      1
#define WL_OK            0
#define WL_ERROR         1
#define MPU6050_DATASIZE 14
#define AS_BUF_LEN       10
/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/

typedef struct {
    uint16_t dataL;
    uint16_t dataR;
} AsRawDataBuf_t;

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

extern int g_flag_usartrec;

extern int   pos_left;
extern float g_vel;
extern int   g_hight;
extern int   wheel_run;

extern int i2cl;
extern int i2cr;
extern int i2cm;
/***********************    FUNCTION PROTOTYPES    ***********************/
void info_atomic_write(int *ptr, int value);
int  info_atomic_read(int *ptr);
#endif
