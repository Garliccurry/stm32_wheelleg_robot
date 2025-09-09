#ifndef __FILTER_H__
#define __FILTER_H__

#include "main.h"
#define FILTER_TF_ROLL 0.1
#define FILTER_TF_JOYY 0.1
#define FILTER_TF_VEL  0.005

typedef struct LPF_TypeDef {
    float    TimeConstant;
    float    output_prev;
    uint32_t us_ts;
} LPF_TypeDef;

typedef struct filter {
    LPF_TypeDef *lpf_roll;
    LPF_TypeDef *lpf_zerobias;
    LPF_TypeDef *lpf_joyy; // TODO(oujiali) 这个还不知道放哪比较好
    LPF_TypeDef *lpf_ang_shaftL;
    LPF_TypeDef *lpf_vel_shaftL;
    LPF_TypeDef *lpf_ang_shaftR;
    LPF_TypeDef *lpf_vel_shaftR;
} FilterSet;

void  Filter_LpfInit(LPF_TypeDef *lpf, float output_preset, float TimeConstant);
float Filter_LpfControl(LPF_TypeDef *lpf, float x);
void  Filter_Init(FilterSet *filter);

#endif
