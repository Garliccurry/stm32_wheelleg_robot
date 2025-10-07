#ifndef __INFO_H__
#define __INFO_H__

/**********************    INCLUDE DIRECTIVES    *************************/
#include "main.h"
#include "utils.h"
#include "filter.h"
#include "pid.h"

#define WLR_OK    0
#define WLR_ERROR 1

#define MPU6050_I2C_DATASIZE   14
#define MPU6050_INT16_DATASIZE 7

#define AS_BUF_LEN  10
#define MPU_BUF_LEN 5

#define PI 3.1415926

#define RX_BUF_SIZE  128U
#define RX_THRESHOLD 2U
#define TX_BUF_SIZE  256U
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
    float    gyroXoffset;
    float    gyroYoffset;
    float    gyroZoffset;
    float    gyroX;
    float    gyroY;
    float    gyroZ;
    float    angleGyroX;
    float    angleGyroY;
    float    angleGyroZ;
    float    angleAccX;
    float    angleAccY;
    float    angleAccZ;
    float    angleX;
    float    angleY;
    float    angleZ;
    float    gyroCoef;
    float    accCoef;
    uint32_t us_ts;
} MpuData_t;

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

typedef struct
{
    uint8_t  ID[2];
    uint16_t position[2];
    uint16_t time[2];
    uint16_t speed[2];
} SCSData_t;

typedef struct
{
    uint8_t buff[RX_BUF_SIZE];
    uint8_t size;
} Command_t;

typedef enum {
    WLR_Idle = 0,
    WLR_Act,
    WLR_On,
    WLR_Off,
} WLR_FlagStatus;

typedef enum {
    WLR_UsartIdle = 0,
    WLR_UsartSend,
    WLR_UsartRecv
} WLR_Usart2Flag;

typedef enum {
    WLR_ERRINIT = 65536, // Error code for placeholder
    WLR_ERR65537,        // can not get battery voltage
    WLR_ERR65538,        // I2c was interrupted for some reason(eg. burning), causing a hardware deadlock
    WLR_ERR65539,        // waiting for SCS serial bus timeout
    WLR_ERR65540,        // LowPassFilter malloc fail
    WLR_ERR65541,        // MPU6050 initial data timeout
    WLR_ERR65542,        // AS5600 initial data timeout
    WLR_ERR65543,        // empty point fail
} WLR_ErrorCode;

extern uint8_t g_flagUart1Recv;
extern uint8_t g_flagUart1Send;
extern uint8_t g_flagUart1Prefix;
extern uint8_t g_flagFatalErr;
extern uint8_t g_flagI2cError;
extern uint8_t g_flagUart2Bus;

extern uint8_t g_flagFocDate;
extern uint8_t g_flagMpuDate;

extern uint32_t g_I2cErrorCount;

extern AsData_t  g_ASdataL;
extern AsData_t  g_ASdataR;
extern MpuData_t g_MPUdata;

extern FilterSet g_lpfSet;
extern PIDSet    g_pidSet;

extern Command_t g_command;

/***********************    FUNCTION PROTOTYPES    ***********************/

void Info_TimerCallbackBattery(void);
void Info_TimerCallbackFatal(void);
void Info_ProcessAffair(void);

void     Info_UsTickIncrease(void);
uint32_t Info_GetUsTick(void);
void     Info_Init(void);
#endif
