#ifndef _ESP8266_USART_H__
#define _ESP8266_USART_H__
#include "main.h"
#include "info.h"
#include "usart.h"

#define ESP8266_UART_TX_BUF_SIZE 64

void ESP8266usart_Printf(char *fmt, ...);

void ESP8266usart_Reset(void);

uint32_t ESP8266usart_GetRecvData(void);
#endif