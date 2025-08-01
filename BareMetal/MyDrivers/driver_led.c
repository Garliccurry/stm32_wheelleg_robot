#include "driver_led.h"
#include "gpio.h"
/**********************************************************************
 * �������ƣ� Led_Init
 * ���������� LED��ʼ������, ��HAL�ĳ�ʼ���������Ѿ����ú���GPIO����,
 *            ���Ա���������дΪ��
 *            ����Ϊ�˲�������stm32cubemx, �˺���Ҳʵ����GPIO������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2025/02/07	     V1.0	  Garlic      ����
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
 * �������ƣ� Led_Control
 * ���������� LED���ƺ���
 * ��������� on-״̬, 1-��, 0-��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2025/02/07	     V1.0	  Garlic      ����
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
 * �������ƣ� Led_Toggle
 * ���������� LED��ת����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2025/02/07	     V1.0	  Garlic      ����
 ***********************************************************************/
int Led_Toggle(void)
{
    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2);
    return 0;
}
