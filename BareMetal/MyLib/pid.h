#ifndef __PID_H__
#define __PID_H__

#include "main.h"
typedef struct PID_TypeDef {
    float    Kp;
    float    Ki;
    float    Kd;
    float    inte_pre;
    float    limit_inte;
    float    limit_out;
    float    error_pre;
    uint32_t time_pre;
} PID_TypeDef;

void  PID_Init(PID_TypeDef *pid, int dir, float p, float i, float d, float l_o, float l_i);
float PID_PosController(PID_TypeDef *pid, float error);
float fPID_IncController(PID_TypeDef *pid, float error);
#endif
