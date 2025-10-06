#include "order.h"

#include "info.h"
#include "esp8266.h"

void Order_ParseCommand(void)
{
    if (g_flagUart1Recv == WLR_Act) {
        uint8_t *start_pos = g_command.buff;
        uint8_t  cmd_len = g_command.size;
        if (g_flagUart1Prefix != WLR_Off) {
            start_pos = (uint8_t *)strstr((char *)g_command.buff, ":");
            RET_IF(start_pos == NULL);
            start_pos++;
            RET_IF(start_pos == NULL);
            cmd_len = g_command.size - (uint8_t)(g_command.buff - start_pos);
        }

        HAL_UART_Transmit(&huart1, start_pos, cmd_len, 1000);
        g_flagUart1Recv = WLR_Idle;
    }
}

void Order_Init(void)
{
    Usart_StartRecvByIdle();
    ESP8266_Init(ESP8266_Sta, ESP8266_Client);
}