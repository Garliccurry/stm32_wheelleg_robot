#ifndef __PID_H__
#define __PID_H__

#include "main.h"
typedef struct PID_TypeDef
{
    float Kp;       //!< ��������(P������)
    float Ki;       //!< �������棨I�����棩
    float Kd;       //!< ΢�����棨D�����棩
    float inte_pre; //!< ���һ�����ַ���ֵ
    float limit_inte;
    float limit_out;
    float error_pre;   //!< ���ĸ������ֵ
    uint32_t time_pre; //!< �ϴ�ִ��ʱ���
} PID_TypeDef;

void PID_Init(PID_TypeDef *pid, int dir, float p, float i, float d, float l_o, float l_i);
float PID_PosController(PID_TypeDef *pid, float error);
float fPID_IncController(PID_TypeDef *pid, float error);
#endif
