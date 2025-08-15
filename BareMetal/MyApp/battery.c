#include "battery.h"
#include "adc.h"
#include "driver_led.h"
#include "filter.h"
#include "log.h"
#include "tim.h"
#include <stdio.h>

static LPF_TypeDef gLpfPower;

void Battery_Init(void)
{
    LPF_Init(&gLpfPower, 7.5, 0.3);
    if (HAL_TIM_Base_Start_IT(&htim5) == HAL_OK) {
        LOG_INFO("Power detection initialization successful!");
    } else {
        LOG_INFO("Power detection initialization failed!");
    }
    HAL_Delay(10);
}

uint32_t Battery_GetData(uint32_t *adc_rlst)
{
    if (HAL_ADC_Start(&hadc1) != HAL_OK) { return WL_ERROR; }
    if (HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK) { return WL_ERROR; }
    *adc_rlst = HAL_ADC_GetValue(&hadc1);
    return WL_OK;
}

void Battery_TimerCallback(void)
{
    uint32_t       ADC_Result = 0;
    static uint8_t TIM5base_cnt = 0;
    static uint8_t target_num = 4;
    if (Battery_GetData(&ADC_Result) != WL_OK) { LOG_ERROR("can not get adc, ret:%d", WL_ERR65537); }
    gVoltage = LowPassFilter(&gLpfPower, (float)ADC_Result / 311);

    if (gVoltage < 7.5f) {
        LOG_INFO("Power voltage:%.2f", gVoltage);
        target_num = 1;
    } else if (gVoltage < 7.6f) {
        target_num = 2;
    } else if (gVoltage < 7.7f) {
        target_num = 3;
    } else {
        target_num = 4;
    }

    if (TIM5base_cnt >= target_num && gflag_fatalerr == WL_OK) {
        TIM5base_cnt = 0;
        Led_Toggle();
    }
    TIM5base_cnt++;
}
