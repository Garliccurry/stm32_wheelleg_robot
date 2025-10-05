#ifndef _ORDER_H__
#define _ORDER_H__
#include "main.h"
#include "usart.h"

extern uint8_t gCommand[RX_BUF_SIZE];

void Order_ParseCommand(void);
#endif