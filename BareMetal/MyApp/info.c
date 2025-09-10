#include "info.h"

#include "adc.h"
#include "i2c.h"
#include "log.h"
#include "tim.h"
#include "filter.h"
#include "battery.h"
#include "driver_led.h"
uint8_t g_flagUsartRec = WLR_StatusIdle;
uint8_t g_flagFatalErr = WLR_StatusOff;
uint8_t g_flagI2cError = WLR_StatusOff;
uint8_t g_flagUart2Bus = WLR_UsartIdle;

uint8_t g_flagFocDate = WLR_StatusIdle;
uint8_t g_flagMpuDate = WLR_StatusIdle;

uint32_t g_I2cErrorCount = 0;

AsData_t  g_ASdataL;
AsData_t  g_ASdataR;
MpuData_t g_MPUdata;

FilterSet g_filter = {0};

float g_Voltage = 7.4;

static uint32_t g_usTick = 0;

static void Info_I2cBusyHandler(void)
{
    // TODO 重新初始化有问题
    if (g_I2cErrorCount > 100) {
        if (g_flagI2cError == WLR_StatusOff) {
            g_flagI2cError = WLR_StatusOn;
            g_I2cErrorCount = 0;
            LOG_ERROR("I2C busy too many times, ret:%d", WLR_ERR65537);
        } else {
            LOG_ERROR("can not re-init i2c bus, ret:%d", WLR_ERR65537);
        }
    }
}

void Info_TimerCallback(void)
{
    static uint8_t TIM5base_cnt = 0;
    static uint8_t target_num = 4;

    if (Battery_GetData() != WLR_OK) { LOG_ERROR("can not get adc value, ret:%d", WLR_ERR65537); }
    target_num = Battery_SetTogFre();

    Info_I2cBusyHandler();

    if (TIM5base_cnt >= target_num && g_flagFatalErr == WLR_StatusOff) {
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

void Info_UsTickIncrease(void)
{
    g_usTick = g_usTick + 10;
}

uint32_t Info_GetUsTick(void)
{
    return g_usTick;
}

void Info_Init(void)
{
    uint32_t ret = HAL_TIM_Base_Start_IT(&htim1);
    if (ret != WLR_OK) {
        LOG_ERROR("Infomation initial fail");
    } else {
        LOG_INFO("Infomation initial successful");
    }
}