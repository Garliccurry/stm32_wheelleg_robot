#ifndef SOFTTIMER_H__
#define SOFTTIMER_H__
#include "main.h"

#define MAX_SOFTWARE_TIMERS 8

// 定时器状态定义
typedef enum {
    TimerStopped = 0,
    TimerRunning,
    TimerPaused
} TimerState;

// 定时器回调函数类型
typedef void (*TimerCallback)(void);

// 定时器结构体
typedef struct {
    uint32_t      startTime;    // 定时器启动时间
    uint32_t      duration;     // 定时器持续时间(ms)
    TimerState    state;        // 定时器状态
    uint8_t       isAutoReload; // 是否自动重载
    TimerCallback callback;     // 定时器超时回调函数
} SoftwareTimer;

// 函数声明
void    SoftwareTimer_Init(void);
uint8_t SoftwareTimer_Create(uint32_t duration, TimerCallback callback, uint8_t isAutoReload);
void    SoftwareTimer_Start(uint8_t timerId);
void    SoftwareTimer_Stop(uint8_t timerId);
void    SoftwareTimer_Pause(uint8_t timerId);
void    SoftwareTimer_Resume(uint8_t timerId);
void    SoftwareTimer_Reset(uint8_t timerId);
void    SoftwareTimer_SetDuration(uint8_t timerId, uint32_t duration);
void    SoftwareTimer_Update(void); // 需要在主循环或定时器中断中调用
#endif