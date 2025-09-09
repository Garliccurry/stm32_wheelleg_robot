#include "pid.h"
#include "foc.h"
#include "usart.h"

void PID_Init(PID_TypeDef *pid, float P, float I, float D, float ramp, float limit_out)
{
    pid->Kp = P;
    pid->Ki = I;
    pid->Kd = D;
    pid->ramp = F_ABS(ramp);
    pid->limit_out = limit_out;

    pid->error_pre = 0.0f;
    pid->inte_pre = 0.0f;
    pid->us_pre = Info_GetUsTick();
}

float PID_PosController(PID_TypeDef *pid, float error)
{
    uint32_t us_now = Info_GetUsTick();

    float Ts = (us_now - pid->us_pre) * 1e-6f;
    if (Ts < 0 || Ts > 0.1f) {
        Ts = 1e-3f;
    }

    float prop = pid->Kp * error;
    float inte = pid->Ki * (error + pid->error_pre) * 0.5f * Ts + pid->inte_pre;
    float deri = pid->Kd * (error - pid->error_pre) / Ts;

    inte = CONSTRAIN(inte, -pid->limit_out, pid->limit_out);
    float output = prop + inte + deri;

    output = CONSTRAIN(output, -pid->limit_out, pid->limit_out);

    float output_rate = (pid->output_pre - output) / Ts;
    if (output_rate > pid->ramp)
        output = pid->output_pre + pid->ramp * Ts;
    else if (output_rate < -pid->ramp)
        output = pid->output_pre - pid->ramp * Ts;

    pid->inte_pre = inte;
    pid->error_pre = error;
    pid->output_pre = output;
    pid->us_pre = us_now;
    return output;
}
