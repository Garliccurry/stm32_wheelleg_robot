#include "info.h"

uint8_t gflag_usartrec = 0;
uint8_t gflag_fatalerr = 0;

float gVoltage = 7.4;

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