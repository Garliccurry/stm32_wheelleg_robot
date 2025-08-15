#include "battery.h"
#include "adc.h"
#include "filter.h"
#include "log.h"
#include "tim.h"
#include <stdio.h>

static LPF_TypeDef gLpfPower;

void Battery_Init(void)
{
    LPF_Init(&gLpfPower, 7.5, 0.3);
    if (HAL_TIM_Base_Start_IT(&htim5) == HAL_OK && HAL_ADC_Start(&hadc1) == HAL_OK) {
        LOG_INFO("Power detection initialization successful!");
    } else {
        LOG_INFO("Power detection initialization failed!");
    }
    HAL_Delay(10);
}

uint32_t Battery_GetData(void)
{
    if (HAL_ADC_PollForConversion(&hadc1, 0) != HAL_OK) { return WL_ERROR; }
    uint32_t adc_result = HAL_ADC_GetValue(&hadc1);
    gVoltage = LowPassFilter(&gLpfPower, (float)adc_result / 311);
    if (HAL_ADC_Start(&hadc1) != HAL_OK) { return WL_ERROR; }
    return WL_OK;
}

uint8_t Battery_SetTogFre(void)
{
    if (gVoltage < 7.5f) {
        LOG_INFO("Power voltage:%.2f", gVoltage);
        return 1;
    } else if (gVoltage < 7.6f) {
        return 2;
    } else if (gVoltage < 7.7f) {
        return 3;
    } else {
        return 4;
    }
}
