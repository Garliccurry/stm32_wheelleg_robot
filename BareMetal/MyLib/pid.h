#ifndef __PID_H__
#define __PID_H__

#include "main.h"
typedef struct PID_TypeDef {
    float Kp;
    float Ki;
    float Kd;
    float inte_pre;
    float ramp;
    float limit_out;
    float error_pre;
    float output_pre;

    uint32_t us_pre;
} PID_TypeDef;

typedef struct PIDSet {
    PID_TypeDef *angle;
    PID_TypeDef *gyro;
    PID_TypeDef *distance;
    PID_TypeDef *speed;
    PID_TypeDef *yaw_angle;
    PID_TypeDef *yaw_gyro;
    PID_TypeDef *lqr_u;
    PID_TypeDef *zerobias;
    PID_TypeDef *roll_angle;
} PIDSet;

void  PID_SetUp(PID_TypeDef *pid, float P, float I, float D, float ramp, float limit_out);
float PID_PosController(PID_TypeDef *pid, float error);
// float PID_IncController(PID_TypeDef *pid, float error);
#endif
