#include "pid.h"
#include "foc.h"
#include "usart.h"
#include <stdlib.h>

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

static void PID_SetUp(PID_TypeDef *pid, float P, float I, float D, float ramp, float limit_out)
{
    pid->Kp = P;
    pid->Ki = I;
    pid->Kd = D;
    pid->ramp = F_ABS(ramp);
    pid->limit_out = limit_out;

    pid->output_pre = 0.0f;
    pid->error_pre = 0.0f;
    pid->inte_pre = 0.0f;
    pid->us_pre = Info_GetUsTick();
}

void PID_Init(void)
{
    uint32_t ret = WLR_OK;
    PIDSet  *pidSet = NULL;
    do {
        pidSet = &g_pidSet;
        BREAK_IF(pidSet == NULL, WLR_ERR65543);

        pidSet->angle = (PID_TypeDef *)malloc(sizeof(PID_TypeDef));
        BREAK_IF(pidSet->angle == NULL, WLR_ERR65542);

        pidSet->distance = (PID_TypeDef *)malloc(sizeof(PID_TypeDef));
        BREAK_IF(pidSet->distance == NULL, WLR_ERR65542);

        pidSet->gyro = (PID_TypeDef *)malloc(sizeof(PID_TypeDef));
        BREAK_IF(pidSet->gyro == NULL, WLR_ERR65542);

        pidSet->lqr_u = (PID_TypeDef *)malloc(sizeof(PID_TypeDef));
        BREAK_IF(pidSet->lqr_u == NULL, WLR_ERR65542);

        pidSet->roll_angle = (PID_TypeDef *)malloc(sizeof(PID_TypeDef));
        BREAK_IF(pidSet->roll_angle == NULL, WLR_ERR65542);

        pidSet->speed = (PID_TypeDef *)malloc(sizeof(PID_TypeDef));
        BREAK_IF(pidSet->speed == NULL, WLR_ERR65542);

        pidSet->yaw_angle = (PID_TypeDef *)malloc(sizeof(PID_TypeDef));
        BREAK_IF(pidSet->yaw_angle == NULL, WLR_ERR65542);

        pidSet->yaw_gyro = (PID_TypeDef *)malloc(sizeof(PID_TypeDef));
        BREAK_IF(pidSet->yaw_gyro == NULL, WLR_ERR65542);

        pidSet->zeropoint = (PID_TypeDef *)malloc(sizeof(PID_TypeDef));
        BREAK_IF(pidSet->zeropoint == NULL, WLR_ERR65542);

    } while (false);
    if (ret != WLR_OK) {
        LOG_ERROR("PID malloc fail, ret:%d", ret);
        return;
    }

    PID_SetUp(pidSet->angle, 0.28, 0, 0, 100000, 8);
    PID_SetUp(pidSet->distance, 0.5, 0, 0, 100000, 8);
    PID_SetUp(pidSet->gyro, 0.02, 0, 0, 100000, 8);
    PID_SetUp(pidSet->lqr_u, 1, 0, 0, 100000, 8);
    PID_SetUp(pidSet->roll_angle, 8, 0, 0, 100000, 450);
    PID_SetUp(pidSet->speed, 0.7, 0, 0, 100000, 8);
    PID_SetUp(pidSet->yaw_angle, 1, 0, 0, 100000, 8);
    PID_SetUp(pidSet->yaw_gyro, 0.04, 0, 0, 100000, 8);
    PID_SetUp(pidSet->zeropoint, 0.002, 0, 0, 100000, 4);
    LOG_INFO("PID control initial successful");
}