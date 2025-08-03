#ifndef __FILTER_H__
#define __FILTER_H__

#include "main.h"
typedef struct LPF_TypeDef {
    float alpha;
    float y_prev;

} LPF_TypeDef;

void  LPF_Init(LPF_TypeDef *lpf, float y_init, float alpha);
float LowPassFilter(LPF_TypeDef *lpf, float x);

#endif
