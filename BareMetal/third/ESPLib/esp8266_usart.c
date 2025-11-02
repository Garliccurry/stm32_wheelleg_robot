#include "esp8266_usart.h"

static uint8_t g_uart_tx_buf[ESP8266_UART_TX_BUF_SIZE];

void ESP8266usart_Printf(char *fmt, ...)
{
    va_list  ap;
    uint16_t len;

    va_start(ap, fmt);
    vsprintf((char *)g_uart_tx_buf, fmt, ap);
    va_end(ap);

    len = strlen((const char *)g_uart_tx_buf);
    HAL_UART_Transmit(&huart1, g_uart_tx_buf, len, HAL_MAX_DELAY);
}

void ESP8266usart_Reset(void)
{
    Command_t *command = Info_GetUsartCommand();
    command->size = 0;
    g_flagUart1Recv = WLR_Idle;
}

uint32_t ESP8266usart_GetRecvData(void)
{
    if (g_flagUart1Recv == WLR_Act) {
        Command_t *command = Info_GetUsartCommand();
        if (command->size < RX_BUF_SIZE && command->size > 0) {
            command->buff[command->size] = '\0';
            return WLR_OK;
        } else {
            command->buff[RX_BUF_SIZE - 1] = '\0';
            return WLR_ERROR;
        }
    }
    return WLR_ERROR;
}