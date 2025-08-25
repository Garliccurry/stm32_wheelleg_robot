#include "info.h"

#include "adc.h"
#include "i2c.h"
#include "log.h"
#include "battery.h"
#include "driver_led.h"
uint8_t gflag_UsartRec = WLStatusIdle;
uint8_t gflag_FatalErr = WLStatusOff;
uint8_t gflag_I2cError = WLStatusOff;
uint8_t gflag_Uart2Bus = WLUsartIdle;

uint32_t gI2cErrorCount = 0;

float gVoltage = 7.4;

void info_atomic_write_s32(int *ptr, int value)
{
    __disable_irq();
    *ptr = value;
    __enable_irq();
}

int info_atomic_read_s32(int *ptr)
{
    int val;
    __disable_irq();
    val = *ptr;
    __enable_irq();
    return val;
}

static void Info_I2cBusyHandler(void)
{
    // TODO 重新初始化有问题
    if (gI2cErrorCount > 100) {
        if (gflag_I2cError == WLStatusOff) {
            gflag_I2cError = WLStatusOn;
            gI2cErrorCount = 0;
            LOG_ERROR("I2C busy too many times, ret:%d", WL_ERR65537);
        } else {
            LOG_ERROR("can not re-init i2c bus, ret:%d", WL_ERR65537);
        }
    }
}

void Info_TimerCallback(void)
{
    static uint8_t TIM5base_cnt = 0;
    static uint8_t target_num = 4;

    if (Battery_GetData() != WL_OK) { LOG_ERROR("can not get adc value, ret:%d", WL_ERR65537); }
    target_num = Battery_SetTogFre();

    Info_I2cBusyHandler();

    if (TIM5base_cnt >= target_num && gflag_FatalErr == WLStatusOff) {
        TIM5base_cnt = 0;
        Led_Toggle();
    }
    TIM5base_cnt++;
}

static void Info_I2cErrRecovery(void)
{
    if (HAL_I2C_GetState(&hi2c3) == HAL_I2C_STATE_BUSY) {
        __HAL_I2C_DISABLE(&hi2c3);
        __HAL_I2C_ENABLE(&hi2c3);
        HAL_Delay(1);
        MX_I2C3_Init();
    }
}
void Info_ProcessAffair(void)
{
    Info_I2cErrRecovery();
}
