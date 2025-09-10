#include "control.h"

#include "sensor.h"
#include "SCSCL.h"
#include "filter.h"
#include "info.h"
#include "tim.h"
#include "log.h"

SCSData_t g_scsdata = {
    .ID[0] = 1,
    .ID[1] = 2,
    .position[0] = 2048,
    .position[1] = 2048,
    .time[0] = 0,
    .time[1] = 0,
    .speed[0] = 1500,
    .speed[1] = 1500,
};

void Control_Init(void)
{
    // HAL_StatusTypeDef status = HAL_OK;
    // status |= HAL_TIM_Base_Start_IT(&htim5);
    // if (status == HAL_OK) {
    //     LOG_INFO("WheelLegged control initialization successful!");
    // }
}

void Control_LeggedBalance(void)
{
    // SyncWritePos(g_scsdata.ID, 2, g_scsdata.position, g_scsdata.time, g_scsdata.speed);
}

void Control_WheelBalance(void)
{
    float shaft_ang_L = Filter_LpfControl(g_filter.lpf_ang_shaftL, g_ASdataL.angle_pre);
    float shaft_ang_R = Filter_LpfControl(g_filter.lpf_ang_shaftR, g_ASdataR.angle_pre);
    float shaft_vel_L = Filter_LpfControl(g_filter.lpf_vel_shaftL, g_ASdataL.shaft_vel);
    float shaft_vel_R = Filter_LpfControl(g_filter.lpf_vel_shaftR, g_ASdataR.shaft_vel);

    float LQR_distance = (-0.5) * (shaft_ang_L + shaft_ang_R);
    float LQR_speed = (-0.5) * (shaft_vel_L + shaft_vel_R);
    float LQR_angle = g_MPUdata.angleY;
    float LQR_groy = g_MPUdata.gyroY;
}

void Control_MotionMove(void)
{
    if (g_flagFocDate == WLR_StatusAct && g_flagMpuDate == WLR_StatusAct) {
        // TODO!(oujiali)进行FOC的平衡控制
        // 1. 更新磁编码器、陀螺仪数据
        // 2. 设置相位、电压，输出foc
        // 3. 计算平衡的pid数据
        Control_WheelBalance();
        Control_LeggedBalance();
        g_flagFocDate = WLR_StatusIdle;
        g_flagMpuDate = WLR_StatusIdle;
    }
}
