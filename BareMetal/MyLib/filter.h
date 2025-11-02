#ifndef __FILTER_H__
#define __FILTER_H__

#include "main.h"
#define FILTER_TF_ROLL   0.1
#define FILTER_TF_JOYY   0.1
#define FILTER_TF_VEL    0.005
#define FILTER_TF_GYRO_Y 0.005

typedef struct LPF_TypeDef {
    float    TimeConstant;
    float    output_prev;
    uint32_t us_ts;
} LPF_TypeDef;

typedef struct FilterSet {
    LPF_TypeDef *roll;
    LPF_TypeDef *zeropoint;
    LPF_TypeDef *joyy; // TODO(oujiali) 这个还不知道放哪比较好
    LPF_TypeDef *ang_shaftL;
    LPF_TypeDef *vel_shaftL;
    LPF_TypeDef *ang_shaftR;
    LPF_TypeDef *vel_shaftR;
    LPF_TypeDef *gyroY;
} FilterSet;

void  Filter_SetUp(LPF_TypeDef *lpf, float output_preset, float TimeConstant);
float Filter_LpfControl(LPF_TypeDef *lpf, float x);
void  Filter_Init(void);

#endif
