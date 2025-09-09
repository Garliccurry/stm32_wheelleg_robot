#include "filter.h"

#include <stdlib.h>
#include "info.h"
#include "utils.h"

void Filter_LpfInit(LPF_TypeDef *lpf, float output_preset, float TimeConstant)
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
    float output = alpha * input + (1.f - alpha) * lpf->output_prev;
    lpf->us_ts = Info_GetUsTick();
    lpf->output_prev = output;
    return output;
}

void Filter_Init(FilterSet *filter)
{
    RET_IF(filter == NULL);
    uint32_t ret = WLR_OK;
    do {
        filter->lpf_roll = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(filter->lpf_roll == NULL, WLR_ERR65540);

        filter->lpf_joyy = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(filter->lpf_joyy == NULL, WLR_ERR65540);

        filter->lpf_zerobias = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(filter->lpf_zerobias == NULL, WLR_ERR65540);

        filter->lpf_ang_shaftL = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(filter->lpf_ang_shaftL == NULL, WLR_ERR65540);

        filter->lpf_ang_shaftR = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(filter->lpf_ang_shaftR == NULL, WLR_ERR65540);

        filter->lpf_vel_shaftL = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(filter->lpf_vel_shaftL == NULL, WLR_ERR65540);

        filter->lpf_vel_shaftR = (LPF_TypeDef *)malloc(sizeof(LPF_TypeDef));
        BREAK_IF(filter->lpf_vel_shaftR == NULL, WLR_ERR65540);
    } while (false);

    if (ret != WLR_OK) {
        LOG_ERROR("LowPassFilter malloc fail");
        return;
    }
    Filter_LpfInit(filter->lpf_roll, 0, FILTER_TF_ROLL);
    Filter_LpfInit(filter->lpf_joyy, 0, FILTER_TF_JOYY);
    // Filter_LpfInit(filter->lpf_zerobias, 0, 0);
    Filter_LpfInit(filter->lpf_ang_shaftL, 0, 0);
    Filter_LpfInit(filter->lpf_ang_shaftR, 0, 0);
    Filter_LpfInit(filter->lpf_vel_shaftL, 0, FILTER_TF_VEL);
    Filter_LpfInit(filter->lpf_vel_shaftR, 0, FILTER_TF_VEL);
    LOG_INFO("LowPassFilter initial succ");
}
