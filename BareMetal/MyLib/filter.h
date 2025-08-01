#ifndef __FILTER_H
#define __FILTER_H

#include "main.h"
typedef struct LPF_TypeDef
{
    float alpha;  //!< ��ͨ�˲�����
    float y_prev; //!< ��һ��ѭ���еĹ��˺��ֵ

} LPF_TypeDef;

void LPF_Init(LPF_TypeDef *lpf, float a);
float LowPassFilter(LPF_TypeDef *lpf, float x);

#endif
