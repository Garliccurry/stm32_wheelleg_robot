#include "foc.h"
#include <math.h>
#include "tim.h"
#include "usart.h"
#include "driver_as5600.h"

static PID_TypeDef g_Vpid_L, g_Vpid_R, g_Ppid_L, g_Ppid_R;

void FOC_MoterInit(Motor_TypeDef *m_L, Motor_TypeDef *m_R,
                   TIM_HandleTypeDef *htim_L,
                   TIM_HandleTypeDef *htim_R,
                   I2C_Device *as_dev_L,
                   I2C_Device *as_dev_R)
{
    if (!(m_L && m_R && htim_L && htim_R))
    {
        printf("FOC init fail\r\n");
        return;
    }
    m_L->htim = htim_L;
    m_R->htim = htim_R;

    PID_Init(&g_Vpid_L, 1, 0.003, 0.1, 0, 1000, 40);
    PID_Init(&g_Vpid_R, -1, 0.003, 0.1, 0, 1000, 40);
    m_L->pid_vel = &g_Vpid_L;
    m_R->pid_vel = &g_Vpid_R;

    PID_Init(&g_Ppid_L, 1, 0.1, 0, 0, 0, 0);
    PID_Init(&g_Ppid_R, -1, 0.1, 0, 0, 0, 0);
    m_L->pid_pos = &g_Ppid_L;
    m_R->pid_pos = &g_Ppid_R;

    m_L->as_dev = as_dev_L;
    m_R->as_dev = as_dev_R;

    FOC_AlignSensor(m_L, POLE_PAIRS, MOTOR_DIR_L, V_POWER);
    FOC_AlignSensor(m_R, POLE_PAIRS, MOTOR_DIR_R, V_POWER);

    MOTOR_L_ENABLE;
    MOTOR_R_ENABLE;

    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    printf("FOC init succ\r\n");
}

static float NormalizeAngle(float ele_angle)
{
    float a = fmodf(ele_angle, _2PI); // ȡ������������ڹ�һ�����г�����ֵ�������֪
    return a >= 0 ? a : (a + _2PI);
}

static float Closeloop_ElecAngle(Motor_TypeDef *m, float angle)
{
    return NormalizeAngle((float)(m->DIR * m->PP) * angle - (m->Z_ElecAngle));
}

void FOC_AlignSensor(Motor_TypeDef *m, float _PP, float _DIR, float _PWR)
{
    m->PP = _PP;
    m->DIR = _DIR;
    m->Vpwr = _PWR;
    m->Z_ElecAngle = Closeloop_ElecAngle(m, 0);
}

// int i = 0;

void SetTorque(Motor_TypeDef *m, float Uq, float angle_el)
{
    uint8_t raw_angle[2];
    AS5600_ReadData(m->as_dev, raw_angle);

    uint16_t angle = raw_angle[0] << 8 | raw_angle[1];
    angle_el = AS5600_GetAng(angle);
    angle_el = Closeloop_ElecAngle(m, angle_el + m->Z_ElecAngle);
    float Ualpha = -Uq * sin(angle_el);
    float Ubeta = Uq * cos(angle_el);

    float Ua = Ualpha + m->Vpwr / 2;
    float Ub = (sqrt(3) * Ubeta - Ualpha) / 2 + m->Vpwr / 2;
    float Uc = (-Ualpha - sqrt(3) * Ubeta) / 2 + m->Vpwr / 2;

    float dc_a = _constrain(Ua / m->Vpwr, 0.0f, 1.0f);
    float dc_b = _constrain(Ub / m->Vpwr, 0.0f, 1.0f);
    float dc_c = _constrain(Uc / m->Vpwr, 0.0f, 1.0f);

    __HAL_TIM_SetCompare(m->htim, TIM_CHANNEL_1, dc_a * 99);
    __HAL_TIM_SetCompare(m->htim, TIM_CHANNEL_2, dc_b * 99);
    __HAL_TIM_SetCompare(m->htim, TIM_CHANNEL_3, dc_c * 99);
}

// void SetTorque(Motor_TypeDef *m, float Uq, float angle_el) {
//
//
//     angle_el = NormalizeAngle(angle_el + m->Z_ElecAngle);
//     float Ualpha =  -Uq*sin(angle_el);
//     float Ubeta =   Uq*cos(angle_el);

//    float Ua = Ualpha + m->Vpwr/2;
//    float Ub = (sqrt(3)*Ubeta-Ualpha)/2 + m->Vpwr/2;
//    float Uc = (-Ualpha-sqrt(3)*Ubeta)/2 + m->Vpwr/2;

//    float dc_a = _constrain(Ua / m->Vpwr, 0.0f , 1.0f );
//    float dc_b = _constrain(Ub / m->Vpwr, 0.0f , 1.0f );
//    float dc_c = _constrain(Uc / m->Vpwr, 0.0f , 1.0f );
//
//    __HAL_TIM_SetCompare(m->htim,TIM_CHANNEL_1,dc_a*99);
//    __HAL_TIM_SetCompare(m->htim,TIM_CHANNEL_2,dc_b*99);
//    __HAL_TIM_SetCompare(m->htim,TIM_CHANNEL_3,dc_c*99);
//}

void FOC_VelocityCloseloop(Motor_TypeDef *m, float target_v, float angle, float vel)
{
    float error = (target_v - vel) * 180.f / _PI;
    float pid_output = PID_PosController(m->pid_vel, error);
    SetTorque(m, pid_output, Closeloop_ElecAngle(m, angle));
    //    printf("%f, %f\n", target_v, vel);
}

void FOC_WheelBalance(Motor_TypeDef *m, float error, float angle)
{
    SetTorque(m, error, Closeloop_ElecAngle(m, angle));
}
void FOC_PositionCloseloop(Motor_TypeDef *m, float motor_target, float angle, float rotation)
{
    float Sensor_Angle, Kp = 0.001;
    Sensor_Angle = rotation * 6.28318530718f + angle;
    SetTorque(m, _constrain(-Kp * (motor_target - m->DIR * Sensor_Angle) * 180 / _PI, -6, 6), Closeloop_ElecAngle(m, Sensor_Angle));
}

float shaft_angle = 0;
unsigned int open_loop_timestamp = 0;

static float Openloop_ElecAngle(Motor_TypeDef *m, float angle)
{
    return NormalizeAngle((float)(m->DIR * m->PP) * angle - (m->Z_ElecAngle));
}

void FOC_VelocityOpenLoop(Motor_TypeDef *m, float target_v)
{
    float Ts = 0.001;

    shaft_angle = shaft_angle + target_v * Ts;
    float Uq = V_POWER / 2;
    SetTorque(m, Uq, Openloop_ElecAngle(m, shaft_angle));
}
