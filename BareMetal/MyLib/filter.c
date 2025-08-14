#include "filter.h"
#include "usart.h"

void LPF_Init(LPF_TypeDef *lpf, float y_init, float alpha)
{
    lpf->alpha = alpha;
    lpf->y_prev = y_init;
}

float LowPassFilter(LPF_TypeDef *lpf, float x)
{
    float y = lpf->alpha * x + (1 - lpf->alpha) * lpf->y_prev;
    lpf->y_prev = y;
    return y;
}
