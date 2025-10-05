#ifndef CONTROL_H__
#define CONTROL_H__
#include "main.h"
#include <stdbool.h>
typedef struct WLrobot {
    int  height;
    int  roll;
    int  linear;
    int  angular;
    int  dir;
    int  dir_last;
    int  joyy;
    int  joyy_last;
    int  joyx;
    int  joyx_last;
    bool go;

    int   jump_flag;        //跳跃时段标识
    float leg_position_add; // roll轴平衡控制量
    int   uncontrolable;    //机身倾角过大导致失控
} WLrobot;

void Control_Init(void);
void Control_LeggedBalance(void);
void Control_MotionMove(void);
#endif