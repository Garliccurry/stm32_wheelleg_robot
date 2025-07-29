#ifndef __FILTER_H
#define __FILTER_H

#include "main.h"
typedef struct LPF_TypeDef
{
    float alpha;    //!< 低通滤波比例
    float y_prev;   //!< 上一个循环中的过滤后的值
    
}LPF_TypeDef;


void vLPF_Init(LPF_TypeDef* lpf, float a);
float fLowPassFilter(LPF_TypeDef* lpf, float x);


#endif
