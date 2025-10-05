/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.h
 * @brief   This file contains all the function prototypes for
 *          the usart.c file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "info.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define RX_BUF_SIZE  32U
#define RX_THRESHOLD 8U
#define TX_BUF_SIZE  256U
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
extern uint8_t gRxBuff[RX_BUF_SIZE];

void Usart_NorLogPrint(uint8_t *ch, uint16_t len);
void Usart_DmaLogPrint(uint8_t *ch, uint16_t len);

void     FTUart_Send(uint8_t *nDat, int nLen);
int      FTUart_Read(uint8_t *nDat, int nLen);
uint32_t FTBus_Delay(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
