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
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

  /* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "shared.h"
  /* USER CODE END Includes */

  extern UART_HandleTypeDef huart1;

  extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define RX_BUF_SIZE 4
  /* USER CODE END Private defines */

  void MX_USART1_UART_Init(void);
  void MX_USART2_UART_Init(void);

  /* USER CODE BEGIN Prototypes */
  extern uint8_t g_rx_buf[RX_BUF_SIZE];

  void vUart_ParseCommand(void);
  void vFTUart_Send(uint8_t *nDat, int nLen);
  int lFTUart_Read(uint8_t *nDat, int nLen);
  void vFTBus_Delay(void);
  /* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
