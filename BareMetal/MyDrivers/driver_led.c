#include "driver_led.h"
#include "gpio.h"
/**********************************************************************
 * 函数名称： Led_Init
 * 功能描述： LED初始化函数, 在HAL的初始化代码里已经配置好了GPIO引脚, 
 *            所以本函数可以写为空
 *            但是为了不依赖于stm32cubemx, 此函数也实现了GPIO的配置
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2025/02/07	     V1.0	  Garlic      创建
 ***********************************************************************/
int Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);

    /*Configure GPIO pin : PE2 */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    return 0;
}

/**********************************************************************
 * 函数名称： Led_Control
 * 功能描述： LED控制函数
 * 输入参数： on-状态, 1-亮, 0-灭
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2025/02/07	     V1.0	  Garlic      创建
 ***********************************************************************/
int Led_Control(int on)
{
    if (on)
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
    return 0;
}

/**********************************************************************
 * 函数名称： Led_Toggle
 * 功能描述： LED反转函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2025/02/07	     V1.0	  Garlic      创建
 ***********************************************************************/
int Led_Toggle(void)
{
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2);
    return 0;
}
