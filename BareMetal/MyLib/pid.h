#ifndef __PID_H__
#define __PID_H__

#include "main.h"
#define PID_RESET_INTE   (1UL << 1)
#define PID_RESET_ERROR  (1UL << 2)
#define PID_RESET_OUTPUT (1UL << 3)
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
    PID_TypeDef *zeropoint;
    PID_TypeDef *roll_angle;
} PIDSet;

float PID_PosController(PID_TypeDef *pid, float error);
void  PID_Init(void);
#endif
