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

typedef struct
{
    uint8_t  ID[2];
    uint16_t position[2];
    uint16_t time[2];
    uint16_t speed[2];
} SCSData_t;

typedef enum {
    WLStatusIdle = 0,
    WLStatusAct = 1,
    WLStatusOn,
    WLStatusOff,
} WlFlagStatus;

typedef enum {
    WLUsartIdle = 0,
    WLUsartSend,
    WLUsartRecv,
    WLUsartBottom,
} WlUsart2Flag;

typedef enum {
    WL_ERRINIT = 65536, // Error code for placeholder
    WL_ERR65537,        // can not get battery voltage
    WL_ERR65538,        // I2c was interrupted for some reason(eg. burning), causing a hardware deadlock
    WL_ERR65539,        // waiting for SCS serial bus timeout
} WlErrorCode;

extern uint8_t gflag_UsartRec;
extern uint8_t gflag_FatalErr;
extern uint8_t gflag_I2cError;
extern uint8_t gflag_Uart2Bus;

extern uint32_t gI2cErrorCount;

extern float gVoltage;

/***********************    FUNCTION PROTOTYPES    ***********************/
void info_atomic_write_s32(int *ptr, int value);
int  info_atomic_read_s32(int *ptr);

void Info_TimerCallback(void);
void Info_ProcessAffair(void);
#endif
