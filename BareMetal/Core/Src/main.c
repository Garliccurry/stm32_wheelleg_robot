/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "battery.h"
#include "log.h"
#include "foc.h"
#include "SCSCL.h"
#include "driver_mpu6050.h"
#include "driver_as5600.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_ADC1_Init();
    MX_I2C1_Init();
    MX_I2C3_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_USART2_UART_Init();
    MX_TIM5_Init();
    /* USER CODE BEGIN 2 */
    Motor_TypeDef motor_L, motor_R;
    uint8_t       MpuRawData[10], AsRawDataL[2], AsRawDataR[2];
    int16_t       acc[3], gyro_y, gyro_z;
    uint16_t      pos_l, pos_r;
    AS5600_Data   AS_L, AS_R;
    I2C_Device   *AS5600_L, *AS5600_R;
    float         ang_roll, ang_pitch, ang_l, ang_r, rot_l, rot_r, vel_l, vel_r, gyr_y, gyr_z;
    int           Lpos_l = -1, Lpos_r = -1;
    float         error_stand, pid_stand, Kp_s = -0.65, Kd_s = -0.00;
    int           count = 0;
    LPF_TypeDef   lpf_gyr_y;

    log_RegisterOutput(Usart_LogPrint);
    log_SetFmt(LOG_FMT_LEVEL_STR | LOG_FMT_TIME_STAMP);

    Battery_Init();
    MPU6050_Init();
    AS5600_Init();
    AS5600_DataInit(&AS_L, &AS_R);
    AS5600_L = AS5600_GetHandle(1);
    AS5600_R = AS5600_GetHandle(2);
    FOC_MoterInit(&motor_L, &motor_R, &htim4, &htim3, AS5600_L, AS5600_R);

    LPF_Init(&lpf_gyr_y, 0, 0.1);

    HAL_UARTEx_ReceiveToIdle_IT(&huart1, g_rx_buf, RX_BUF_SIZE);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */
        vUart_ParseCommand();

        MPU6050_ReadData(MpuRawData);
        acc[0] = MpuRawData[0] << 8 | MpuRawData[1];
        acc[1] = MpuRawData[2] << 8 | MpuRawData[3];
        acc[2] = MpuRawData[4] << 8 | MpuRawData[5];
        gyro_y = MpuRawData[6] << 8 | MpuRawData[7];
        gyro_z = MpuRawData[8] << 8 | MpuRawData[9];
        gyr_y = gyro_y / 512.0f;
        gyr_y = LowPassFilter(&lpf_gyr_y, gyr_y);
        MPU6050_ParseData(acc, &ang_roll, &ang_pitch);

        AS5600_ReadData(AS5600_L, AsRawDataL);
        AS5600_ReadData(AS5600_R, AsRawDataR);
        pos_l = AsRawDataL[0] << 8 | AsRawDataL[1];
        pos_r = AsRawDataR[0] << 8 | AsRawDataR[1];
        AS5600_ParseData(&AS_L, pos_l, &ang_l, &rot_l, &vel_l);
        AS5600_ParseData(&AS_R, pos_r, &ang_r, &rot_r, &vel_r);

        if (wheel_run != 0) {
            //            printf("%f, %f\r\n", ang_l, vel_l);
            //            FOC_VelocityCloseloop(&motor_L, g_vel, ang_l, vel_l);
            //            FOC_VelocityCloseloop(&motor_R, -g_vel, ang_r, vel_r);
            error_stand = ang_roll_zero - ang_roll;
            pid_stand = Kp_s * error_stand + Kd_s * gyr_y;
            printf("%f \r\n", pid_stand);
            FOC_WheelBalance(&motor_L, pid_stand, ang_l);
            FOC_WheelBalance(&motor_R, pid_stand, ang_r);
        }

        if (count == 1000) {
            //            Lpos_l = ReadPos(1);
            //            Lpos_r = ReadPos(2);
            // WritePos(1, 2048 + g_hight, 0, 1500);
            // WritePos(2, 2048 - g_hight, 0, 1500);
            // printf("x\n");
            count = 0;
        }
        count++;
        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
