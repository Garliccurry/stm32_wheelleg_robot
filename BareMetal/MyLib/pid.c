#include "pid.h"
#include "foc.h"
#include "usart.h"

void vPID_Init(PID_TypeDef* pid,int dir, float p, float i, float d, float l_o,float l_i)
{
    // 参数配置
    pid->Kp = dir * p;
    pid->Ki = dir * i;
    pid->Kd = dir * d;
    pid->limit_out = l_o;
    pid->limit_inte = l_i;

    // 误差历史初始化
    pid->error_pre = 0.0f;

    // 积分残留量初始化
    pid->inte_pre = 0.0f;
}


float fPID_PosController(PID_TypeDef* pid, float error)
{
    float Ts = 0.001;
    
    float prop = pid->Kp * error;
    float inte = pid->Ki * (error + pid->error_pre) * 0.5f * Ts + pid->inte_pre;
    float deri = pid->Kd * (error - pid->error_pre) / Ts;
    
	inte = _constrain(inte, -pid->limit_inte, pid->limit_inte);
    float output = prop + inte + deri;
    output = _constrain(output, -pid->limit_out, pid->limit_out);
    
    
    pid->inte_pre = inte;
    pid->error_pre = error;
    
    return output;
}
