#include "filter.h"
#include "usart.h"

void vLPF_Init(LPF_TypeDef* lpf, float a)
{
    lpf->alpha = a;
    lpf->y_prev = 0;
}

float fLowPassFilter(LPF_TypeDef* lpf, float x)
{
    float y = lpf->alpha * x + (1 - lpf->alpha) * lpf->y_prev;
    lpf->y_prev = y;
    return y;
}
