#include "info.h"

int g_flag_usartrec = 0;

int   pos_left = 0;
float g_vel = 0;
int   g_hight = 0;
int   wheel_run = 0;
int   i2cl = 0;
int   i2cr = 0;
int   i2cm = 0;

void info_atomic_write(int *ptr, int value)
{
    __disable_irq();
    *ptr = value;
    __enable_irq();
}

int info_atomic_read(int *ptr)
{
    int val;
    __disable_irq();
    val = *ptr;
    __enable_irq();
    return val;
}