#include "battery.h"
#include "adc.h"
#include "driver_led.h"
#include "filter.h"
#include "log.h"
#include "tim.h"
#include <stdio.h>

static LPF_TypeDef g_ltf_pwr;

void Battery_Init(void)
{
    LPF_Init(&g_ltf_pwr, 7.5, 0.3);
    if (HAL_TIM_Base_Start_IT(&htim5) == HAL_OK) {
        LOG_INFO("Power detection initialization successful!");
    } else {
        LOG_INFO("Power detection initialization failed!");
    }
    HAL_Delay(10);
}

void Battery_TimerCallback(void)
{
    uint32_t   ADC_Result = 0;
    float      Voltage = 0;
    static int i = 0;
    HAL_ADC_Start(&hadc1);

    if (HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK) {
        ADC_Result = HAL_ADC_GetValue(&hadc1);
        Voltage = LowPassFilter(&g_ltf_pwr, (float)ADC_Result / 311);
        if (i >= 4 || i < 0) {
            i = 0;
            Led_Toggle();
            // LOG_INFO("Power voltage:%.2f", Voltage);
        }
        i++;
    }
}
