#ifndef __PID_H__
#define __PID_H__

#include "main.h"
typedef struct PID_TypeDef
{
    float Kp; //!< 比例增益(P环增益)
    float Ki; //!< 积分增益（I环增益）
    float Kd; //!< 微分增益（D环增益）
    float inte_pre; //!< 最后一个积分分量值
    float limit_inte; 
    float limit_out; 
    float error_pre; //!< 最后的跟踪误差值
    uint32_t time_pre; //!< 上次执行时间戳
}PID_TypeDef;

void vPID_Init(PID_TypeDef* pid,int dir, float p, float i, float d, float l_o,float l_i);
float fPID_PosController(PID_TypeDef* pid, float error);
float fPID_IncController(PID_TypeDef* pid, float error);
#endif
