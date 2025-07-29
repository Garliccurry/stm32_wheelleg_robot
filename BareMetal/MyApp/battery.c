#include "battery.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "driver_led.h"
#include <stdio.h>

void vBattery_Init(void)
{
    printf("adc init succ\r\n");
	HAL_TIM_Base_Start_IT(&htim5);
}

void vBattery_TimerCallback(void)
{
	uint32_t ADC_Result = 0;
    float Voltage = 0;
	static int i;
    HAL_ADC_Start(&hadc1);

    if (HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK)
    {
        ADC_Result = HAL_ADC_GetValue(&hadc1);
        Voltage = (float)ADC_Result/311;
		if(i >= 4 || i < 0)
		{
			i = 0;
			Led_Toggle();
            printf("adc:%.2f V\r\n",Voltage);
		}
		i++;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */

  /* USER CODE BEGIN Callback 1 */
	if(htim->Instance == TIM5)
	{
		vBattery_TimerCallback();
	}
  /* USER CODE END Callback 1 */
}


