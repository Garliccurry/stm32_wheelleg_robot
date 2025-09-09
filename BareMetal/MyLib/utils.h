#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdbool.h>
#include "main.h"
#include "log.h"

#define ATOMIC_WRITE(ptr, value) \
    do {                         \
        __disable_irq();         \
        *(ptr) = (value);        \
        __enable_irq();          \
    } while (0)

#define ATOMIC_READ(ptr) ({ \
    __typeof__(*(ptr)) val; \
    __disable_irq();        \
    val = *(ptr);           \
    __enable_irq();         \
    val;                    \
})

#define RET_IF_1(cond) \
    if ((cond) == true) { return; }
#define RET_IF_2(cond, val) \
    if ((cond) == true) { return (val); }
#define RET_IF_N(_1, _2, N, ...) RET_IF_##N
#define RET_IF(...)              RET_IF_N(__VA_ARGS__, 2, 1)(__VA_ARGS__)

#define BREAK_IF_1(cond) \
    if ((cond) == true) { break; }
#define BREAK_IF_2(cond, val) \
    if ((cond) == true) {     \
        ret = (val);          \
        break;                \
    }
#define BREAK_IF_N(_1, _2, N, ...) BREAK_IF_##N
#define BREAK_IF(...)              BREAK_IF_N(__VA_ARGS__, 2, 1)(__VA_ARGS__)

#define F_ABS(val) (((val) >= 0) ? (val) : (-val))
static inline void Utils_AtomicSetU8(uint8_t *ptr, uint8_t value)
{
    __disable_irq();
    *ptr = value;
    __enable_irq();
}

static inline uint8_t Utils_AtomicGetU8(uint8_t *ptr)
{
    int val;
    __disable_irq();
    val = *ptr;
    __enable_irq();
    return val;
}

#endif