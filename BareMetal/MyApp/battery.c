#include "battery.h"
#include "adc.h"
#include "filter.h"
#include "log.h"
#include "tim.h"
#include <stdio.h>

static LPF_TypeDef g_LpfPower;

void Battery_Init(void)
{
    Filter_SetUp(&g_LpfPower, 7.5, 0.3);
    if (HAL_ADC_Start(&hadc1) == HAL_OK) {
        LOG_INFO("Power detection initialization successful!");
    } else {
        LOG_INFO("Power detection initialization failed!");
    }
    HAL_Delay(10);
}

uint32_t Battery_GetData(void)
{
    if (HAL_ADC_PollForConversion(&hadc1, 0) != HAL_OK) { return WLR_ERROR; }
    uint32_t adc_result = HAL_ADC_GetValue(&hadc1);
    g_Voltage = Filter_LpfControl(&g_LpfPower, (float)adc_result / 311);
    if (HAL_ADC_Start(&hadc1) != HAL_OK) { return WLR_ERROR; }
    return WLR_OK;
}

uint8_t Battery_SetTogFre(void)
{
    if (g_Voltage < 7.5f) {
        LOG_INFO("Power voltage:%.2f", g_Voltage);
        return 1;
    } else if (g_Voltage < 7.6f) {
        return 2;
    } else if (g_Voltage < 7.7f) {
        return 3;
    } else {
        return 4;
    }
}
