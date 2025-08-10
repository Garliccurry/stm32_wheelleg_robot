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
#define AS5600_TEST 1
/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/
typedef struct
{
    float roll;
    float pitch;
    float yaw;
} MPU6050_Data;

typedef struct
{
    float    angle_pre;
    uint32_t angle_pre_ts;
    float    Gvel_angle_pre;
    uint32_t Gvel_angle_pre_ts;
    float    rota_pre;
    float    Gvel_rota_pre;
} AS5600_Data;

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
