#include "filter.h"

#include <stdlib.h>
#include "info.h"
#include "utils.h"

void Filter_SetUp(LPF_TypeDef *lpf, float output_preset, float TimeConstant)
{
    lpf->TimeConstant = TimeConstant;
    lpf->output_prev = output_preset;
    lpf->us_ts = 0;
}

float Filter_LpfControl(LPF_TypeDef *lpf, float input)
{
    if (lpf->TimeConstant == 0) {
        return input;
    }

    uint32_t now_us = Info_GetUsTick();

    float dt = (now_us - lpf->us_ts) * 1e-6f;
    if (dt < 0) {
        dt = 1e-3f;
    } else if (dt > 0.3f || lpf->TimeConstant <= 0) {
        lpf->us_ts = now_us;
        lpf->output_prev = input;
        return input;
    }

    float alpha = lpf->TimeConstant / (lpf->TimeConstant + dt);
    float output = (1.f - alpha) * input + alpha * lpf->output_prev;
    lpf->us_ts = Info_GetUsTick();
    lpf->output_prev = output;
    return output;
}

void Filter_Init(void)
{
    uint32_t   ret = WLR_OK;
    FilterSet *lpfSet = Info_GetFilterSet();
    do {
        lpfSet->roll = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(lpfSet->roll == NULL, WLR_ERR65540);

        lpfSet->joyy = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(lpfSet->joyy == NULL, WLR_ERR65540);

        lpfSet->zeropoint = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(lpfSet->zeropoint == NULL, WLR_ERR65540);

        lpfSet->ang_shaftL = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(lpfSet->ang_shaftL == NULL, WLR_ERR65540);

        lpfSet->ang_shaftR = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(lpfSet->ang_shaftR == NULL, WLR_ERR65540);

        lpfSet->vel_shaftL = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(lpfSet->vel_shaftL == NULL, WLR_ERR65540);

        lpfSet->vel_shaftR = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(lpfSet->vel_shaftR == NULL, WLR_ERR65540);

        lpfSet->gyroY = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(lpfSet->gyroY == NULL, WLR_ERR65540);
    } while (false);

    if (ret != WLR_OK) {
        LOG_ERROR("LowPassFilter malloc fail, ret:%d", ret);
        return;
    }
    Filter_SetUp(lpfSet->roll, 0, FILTER_TF_ROLL);
    Filter_SetUp(lpfSet->joyy, 0, FILTER_TF_JOYY);
    Filter_SetUp(lpfSet->zeropoint, 0, 0);
    Filter_SetUp(lpfSet->ang_shaftL, 0, 0);
    Filter_SetUp(lpfSet->ang_shaftR, 0, 0);
    Filter_SetUp(lpfSet->vel_shaftL, 0, FILTER_TF_VEL);
    Filter_SetUp(lpfSet->vel_shaftR, 0, FILTER_TF_VEL);
    Filter_SetUp(lpfSet->gyroY, 0, FILTER_TF_GYRO_Y);
    LOG_INFO("LowPassFilter initial succ");
}
