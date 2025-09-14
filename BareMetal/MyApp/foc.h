#ifndef __FOC_H__
#define __FOC_H__

#include "main.h"
#include "device.h"
#include "pid.h"
#include "filter.h"

#define CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

#define _2_SQRT3 1.15470053838f
#define _SQRT3   1.73205080757f
#define _1_SQRT3 0.57735026919f
#define _SQRT3_2 0.86602540378f
#define _SQRT2   1.41421356237f
#define _120_D2R 2.09439510239f
#define _PI      3.14159265359f
#define _PI_2    1.57079632679f
#define _PI_3    1.0471975512f
#define _2PI     6.28318530718f
#define _3PI_2   4.71238898038f
#define _PI_6    0.52359877559f

#define V_POWER       7.f
#define POLE_PAIRS    7.f
#define MOTOR_DIR_L   -1.f
#define MOTOR_DIR_R   -1.f
#define ang_roll_zero -8.8f

#define MOTOR_L_ENABLE  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET)
#define MOTOR_R_ENABLE  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET)
#define MOTOR_L_DISABLE HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET)
#define MOTOR_R_DISABLE HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET)
#define MOTOR_L_TOGGLE  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_11)
#define MOTOR_R_TOGGLE  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15)

typedef struct Motor_TypeDef {
    TIM_HandleTypeDef *htim;

    float PP;
    float DIR;
    float Vpwr;
    float Z_ElecAngle;

    PID_TypeDef *pid_vel;
    PID_TypeDef *pid_pos;

    I2cDevice_t *as_dev;
} Motor_TypeDef;

void FOC_MoterInit(Motor_TypeDef *m_L, Motor_TypeDef *m_R,
                   TIM_HandleTypeDef *htim_L,
                   TIM_HandleTypeDef *htim_R);
void SetTorque(Motor_TypeDef *m, float Uq, float angle_el);
void FOC_AlignSensor(Motor_TypeDef *m, float PP, float DIR, float Vpwr);
void FOC_VelocityCloseloop(Motor_TypeDef *m, float target_v, float angle, float vel);
void FOC_WheelBalance(Motor_TypeDef *m, float target, float angle);
void FOC_PositionCloseloop(Motor_TypeDef *m, float motor_target, float angle, float rotation);
void FOC_VelocityOpenLoop(Motor_TypeDef *m, float target_v);
#endif
