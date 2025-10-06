#include "softtimer.h"

#include <string.h>

#include "battery.h"
#include "info.h"
#include "log.h"

// 软件定时器数组
static SoftwareTimer timers[MAX_SOFTWARE_TIMERS] = {0};
static uint8_t       timerCount = 0;

void SoftwareTimer_test(void)
{
    LOG_DEBUG("test software timer");
}

// 获取当前系统时间(ms)
static inline uint32_t GetCurrentTime(void)
{
    return HAL_GetTick();
}

// 初始化软件定时器模块
void SoftwareTimer_Init(void)
{
    memset(timers, 0, sizeof(timers));
    timerCount = 0;

    Battery_Init();

    uint8_t timer1 = SoftwareTimer_Create(250, Info_TimerCallbackBattery, WLR_OK);
    timerCount++;
    uint8_t timer2 = SoftwareTimer_Create(100, Info_TimerCallbackFatal, WLR_OK);
    timerCount++;
    SoftwareTimer_Start(timer1);
    SoftwareTimer_Start(timer2);
}

// 创建软件定时器
uint8_t SoftwareTimer_Create(uint32_t duration, TimerCallback callback, uint8_t isAutoReload)
{
    if (timerCount >= MAX_SOFTWARE_TIMERS) {
        LOG_ERROR("Reaching the maximum number of timers");
        return 0xFF;
    }

    if (duration == 0 || callback == NULL) {
        LOG_ERROR("Invalid timer parameter");
        return 0xFF;
    }

    timers[timerCount].duration = duration;
    timers[timerCount].callback = callback;
    timers[timerCount].isAutoReload = isAutoReload;
    timers[timerCount].state = TimerStopped;
    timers[timerCount].startTime = 0;

    return timerCount++;
}

// 启动定时器
void SoftwareTimer_Start(uint8_t timerId)
{
    if (timerId >= timerCount) return;

    timers[timerId].startTime = GetCurrentTime();
    timers[timerId].state = TimerRunning;
}

// 停止定时器
void SoftwareTimer_Stop(uint8_t timerId)
{
    if (timerId >= timerCount) return;

    timers[timerId].state = TimerStopped;
}

// 暂停定时器
void SoftwareTimer_Pause(uint8_t timerId)
{
    if (timerId >= timerCount) return;

    if (timers[timerId].state == TimerRunning) {
        // 计算剩余时间并保存
        uint32_t elapsed = GetCurrentTime() - timers[timerId].startTime;
        if (elapsed < timers[timerId].duration) {
            timers[timerId].duration -= elapsed;
        } else {
            timers[timerId].duration = 0;
        }
        timers[timerId].state = TimerPaused;
    }
}

// 恢复定时器
void SoftwareTimer_Resume(uint8_t timerId)
{
    if (timerId >= timerCount) return;

    if (timers[timerId].state == TimerPaused) {
        timers[timerId].startTime = GetCurrentTime();
        timers[timerId].state = TimerRunning;
    }
}

// 重置定时器
void SoftwareTimer_Reset(uint8_t timerId)
{
    if (timerId >= timerCount) return;

    timers[timerId].startTime = GetCurrentTime();
    if (timers[timerId].state != TimerStopped) {
        timers[timerId].state = TimerRunning;
    }
}

// 设置定时器持续时间
void SoftwareTimer_SetDuration(uint8_t timerId, uint32_t duration)
{
    if (timerId >= timerCount) return;

    timers[timerId].duration = duration;
    if (timers[timerId].state == TimerRunning) {
        timers[timerId].startTime = GetCurrentTime();
    }
}

// 更新所有定时器状态(需要在主循环或定时器中断中定期调用)
void SoftwareTimer_Update(void)
{
    uint32_t currentTime = GetCurrentTime();

    for (uint8_t i = 0; i < timerCount; i++) {
        if (timers[i].state != TimerRunning) continue;

        // 检查定时器是否超时
        if ((currentTime - timers[i].startTime) >= timers[i].duration) {
            // 执行回调函数
            if (timers[i].callback != NULL) {
                timers[i].callback();
            }

            // 处理自动重载或停止
            if (timers[i].isAutoReload == WLR_OK) {
                timers[i].startTime = currentTime;
            } else {
                timers[i].state = TimerStopped;
            }
        }
    }
}