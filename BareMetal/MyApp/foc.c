#include "foc.h"
#include <math.h>
#include "tim.h"
#include "log.h"
#include "driver_as5600.h"

static float NormalizeAngle(float ele_angle)
{
    float a = fmodf(ele_angle, _2PI);
    return a >= 0 ? a : (a + _2PI);
}

static float Closeloop_ElecAngle(Motor_TypeDef *m, float angle)
{
    return NormalizeAngle((float)(m->sDIR * m->PP) * angle - (m->Z_ElecAngle));
}

static void FOC_AlignSensor(Motor_TypeDef *m, float PP, float sDIR, float mDIR, float Vpwr)
{
    m->PP = PP;
    m->sDIR = sDIR;
    m->mDIR = mDIR;
    m->Vpwr = Vpwr;
    m->Z_ElecAngle = 0;
}

// int i = 0;

// void SetTorque(Motor_TypeDef *m, float Uq, float angle_el)
// {
//     static uint8_t raw_angle[2];
//     AS5600_ReadData(m->as_dev, raw_angle);

//     uint16_t angle = raw_angle[0] << 8 | raw_angle[1];
//     angle_el = AS5600_GetAngFromRaw(angle);
//     angle_el = Closeloop_ElecAngle(m, angle_el + m->Z_ElecAngle);
//     float Ualpha = -Uq * sin(angle_el);
//     float Ubeta = Uq * cos(angle_el);

//     float Ua = Ualpha + m->Vpwr / 2;
//     float Ub = (sqrt(3) * Ubeta - Ualpha) / 2 + m->Vpwr / 2;
//     float Uc = (-Ualpha - sqrt(3) * Ubeta) / 2 + m->Vpwr / 2;

//     float dc_a = CONSTRAIN(Ua / m->Vpwr, 0.0f, 1.0f);
//     float dc_b = CONSTRAIN(Ub / m->Vpwr, 0.0f, 1.0f);
//     float dc_c = CONSTRAIN(Uc / m->Vpwr, 0.0f, 1.0f);

//     __HAL_TIM_SetCompare(m->htim, TIM_CHANNEL_1, dc_a * 99);
//     __HAL_TIM_SetCompare(m->htim, TIM_CHANNEL_2, dc_b * 99);
//     __HAL_TIM_SetCompare(m->htim, TIM_CHANNEL_3, dc_c * 99);
// }

void SetTorque(Motor_TypeDef *m, float Uq, float angle_el)
{
    // LOG_DEBUG("%f,%f", Uq, angle_el); // TODO(oujiali)正反转有问题
    angle_el = NormalizeAngle(angle_el);
    float Ualpha = -Uq * sin(angle_el);
    float Ubeta = Uq * cos(angle_el);

    float Ua = Ualpha + m->Vpwr / 2;
    float Ub = (sqrt(3) * Ubeta - Ualpha) / 2 + m->Vpwr / 2;
    float Uc = (-Ualpha - sqrt(3) * Ubeta) / 2 + m->Vpwr / 2;

    float dc_a = CONSTRAIN(Ua / m->Vpwr, 0.0f, 1.0f);
    float dc_b = CONSTRAIN(Ub / m->Vpwr, 0.0f, 1.0f);
    float dc_c = CONSTRAIN(Uc / m->Vpwr, 0.0f, 1.0f);

    __HAL_TIM_SetCompare(m->htim, TIM_CHANNEL_1, dc_a * 99);
    __HAL_TIM_SetCompare(m->htim, TIM_CHANNEL_2, dc_b * 99);
    __HAL_TIM_SetCompare(m->htim, TIM_CHANNEL_3, dc_c * 99);
}

void FOC_VelocityCloseloop(Motor_TypeDef *m, float target_v, float angle, float vel)
{
    float error = (target_v - vel) * 180.f / _PI;
    float pid_output = PID_PosController(m->pid_vel, error);
    SetTorque(m, pid_output, Closeloop_ElecAngle(m, angle));
}

void FOC_WheelBalance(Motor_TypeDef *m, float target, float angle)
{
    float m_target = (m->mDIR >= 0) ? target : -target;
    SetTorque(m, m_target, Closeloop_ElecAngle(m, angle));
}

void FOC_PositionCloseloop(Motor_TypeDef *m, float motor_target, float angle, float rotation)
{
    float Sensor_Angle, Kp = 0.001;
    Sensor_Angle = rotation * 6.28318530718f + angle;
    SetTorque(m, CONSTRAIN(-Kp * (motor_target - m->sDIR * Sensor_Angle) * 180 / _PI, -6, 6), Closeloop_ElecAngle(m, Sensor_Angle));
}

#ifdef TEST
static float g_shaft_angle = 0;

static float Openloop_ElecAngle(Motor_TypeDef *m, float angle)
{
    return NormalizeAngle((float)(m->DIR * m->PP) * angle - (m->Z_ElecAngle));
}

void FOC_VelocityOpenLoop(Motor_TypeDef *m, float target_v)
{
    float Ts = 0.001;

    g_shaft_angle = g_shaft_angle + target_v * Ts;
    // LOG_DEBUG("%f\r\n", g_shaft_angle);
    float Uq = FOC_VOLT_POWER / 2;
    SetTorque(m, Uq, Openloop_ElecAngle(m, g_shaft_angle));
}
#endif

void FOC_MoterInit(Motor_TypeDef *m_L, Motor_TypeDef *m_R,
                   TIM_HandleTypeDef *htim_L,
                   TIM_HandleTypeDef *htim_R)
{
    if (!(m_L && m_R && htim_L && htim_R)) {
        LOG_INFO("FOC initialization failed!");
        return;
    }
    m_L->htim = htim_L;
    m_R->htim = htim_R;

    FOC_AlignSensor(m_L, FOC_POLE_PAIRS, FOC_SENSOR_DIR_L, FOC_MOTOR_DIR_L, FOC_VOLT_POWER);
    FOC_AlignSensor(m_R, FOC_POLE_PAIRS, FOC_SENSOR_DIR_R, FOC_MOTOR_DIR_R, FOC_VOLT_POWER);

    m_L->as_dev = AS5600_GetHandle(AS5600Left);
    m_R->as_dev = AS5600_GetHandle(AS5600Right);

    HAL_StatusTypeDef status = HAL_OK;

    MOTOR_L_ENABLE;
    MOTOR_R_ENABLE;
    status |= HAL_TIM_PWM_Start(htim_L, TIM_CHANNEL_1);
    status |= HAL_TIM_PWM_Start(htim_L, TIM_CHANNEL_2);
    status |= HAL_TIM_PWM_Start(htim_L, TIM_CHANNEL_3);

    status |= HAL_TIM_PWM_Start(htim_R, TIM_CHANNEL_1);
    status |= HAL_TIM_PWM_Start(htim_R, TIM_CHANNEL_2);
    status |= HAL_TIM_PWM_Start(htim_R, TIM_CHANNEL_3);

    SetTorque(m_L, m_L->Vpwr, _3PI_2);
    SetTorque(m_R, m_R->Vpwr, _3PI_2);
    HAL_Delay(100);

    LOG_DEBUG("Left Zero ele angle ret:%d", status);
    uint8_t  asrawdataL[AS5600_I2C_DATASIZE] = {0}, asrawdataR[AS5600_I2C_DATASIZE] = {0};
    uint16_t asu16dataL, asu16dataR;
    status |= AS5600_NorReadData(m_L->as_dev, asrawdataL);
    status |= AS5600_NorReadData(m_R->as_dev, asrawdataR);
    asu16dataL = (uint16_t)asrawdataL[0] << 8 | asrawdataL[1];
    asu16dataR = (uint16_t)asrawdataR[0] << 8 | asrawdataR[1];
    float angleL = AS5600_GetAngFromRaw(asu16dataL);
    float angleR = AS5600_GetAngFromRaw(asu16dataR);
    LOG_DEBUG("Left Zero ele angle:%f,%f", m_L->Z_ElecAngle, angleL);
    m_L->Z_ElecAngle = Closeloop_ElecAngle(m_L, angleL);
    m_R->Z_ElecAngle = Closeloop_ElecAngle(m_R, angleR);
    LOG_DEBUG("Left Zero ele angle:%f,%f", m_L->Z_ElecAngle, angleL);

    SetTorque(m_L, 0, 0);
    SetTorque(m_R, 0, 0);
    HAL_Delay(100);

    if (status != HAL_OK) {
        LOG_INFO("FOC initialization failed, ret: %d", status);
    } else {
        LOG_INFO("FOC initialization successful!");
    }
}
