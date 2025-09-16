#include "control.h"

#include "sensor.h"
#include "SCSCL.h"
#include "filter.h"
#include "info.h"
#include "tim.h"
#include "log.h"
#include "foc.h"

static SCSData_t scsdata;

static WLrobot       wlrobot;
static Motor_TypeDef motor_L, motor_R;

void Control_Init(void)
{
    // memset(&wlrobot, 0, sizeof(WLrobot));
    wlrobot.go = false;
    wlrobot.joyx = 0;
    wlrobot.joyx_last = 0;
    wlrobot.joyy = 0;
    wlrobot.joyy_last = 0;

    FOC_MoterInit(&motor_L, &motor_R, &htim4, &htim3);
}

void Control_LeggedBalance(void)
{
    scsdata.ID[0] = 1;
    scsdata.ID[1] = 2;
    scsdata.position[0] = 2048;
    scsdata.position[1] = 2048;
    scsdata.time[0] = 0;
    scsdata.time[1] = 0;
    scsdata.speed[0] = 1500;
    scsdata.speed[1] = 1500;

    SyncWritePos(scsdata.ID, 2, scsdata.position, scsdata.time, scsdata.speed);
}

static float Control_WheelGetLQR(void)
{
    static float angle_zeropoint = -2.25f;
    static float distance_zeropoint = 0;
    static float robot_speed = 0;      //记录当前轮部转速
    static float robot_speed_last = 0; //记录上一时刻的轮部转速

    FilterSet *lpfSet = &g_lpfSet;
    PIDSet    *pidSet = &g_pidSet;

    float shaft_ang_L = Filter_LpfControl(lpfSet->ang_shaftL, g_ASdataL.angle_pre);
    float shaft_ang_R = Filter_LpfControl(lpfSet->ang_shaftR, g_ASdataR.angle_pre);
    float shaft_vel_L = Filter_LpfControl(lpfSet->vel_shaftL, g_ASdataL.shaft_vel);
    float shaft_vel_R = Filter_LpfControl(lpfSet->vel_shaftR, g_ASdataR.shaft_vel);

    float LQR_distance = (-0.5) * (shaft_ang_L + shaft_ang_R);
    float LQR_speed = (-0.5) * (shaft_vel_L + shaft_vel_R);
    float LQR_angle = g_MPUdata.angleY;
    float LQR_gyro = g_MPUdata.gyroY;
    float LQR_u = 0;

    float angle_control = PID_PosController(pidSet->angle, LQR_angle - angle_zeropoint);
    float gyro_control = PID_PosController(pidSet->gyro, LQR_gyro);

    if (wlrobot.joyy != WLR_Off) {
        distance_zeropoint = LQR_distance;
        pidSet->lqr_u->error_pre = 0;
    }
    if ((wlrobot.joyx_last != WLR_Off && wlrobot.joyx == WLR_Off) || (wlrobot.joyy_last != WLR_Off && wlrobot.joyy == WLR_Off)) {
        if (F_ABS(LQR_speed) < 0.5f) {
            distance_zeropoint = LQR_distance;
        }
    }
    if (F_ABS(LQR_speed) > 15.f) {
        distance_zeropoint = LQR_distance;
    }

    float lpf_joyy_value = Filter_LpfControl(lpfSet->joyy, (float)wlrobot.joyy);
    float speed_control = PID_PosController(pidSet->speed, LQR_speed - 0.1 * lpf_joyy_value);
    float distance_control = PID_PosController(pidSet->distance, LQR_distance - distance_zeropoint);

    robot_speed_last = robot_speed;
    robot_speed = LQR_speed;
    if (F_ABS(robot_speed - robot_speed_last) > 10 || F_ABS(robot_speed) > 50 || (wlrobot.jump_flag != WLR_Off)) {
        distance_zeropoint = LQR_distance;
        LQR_u = angle_control + gyro_control;
        pidSet->lqr_u->error_pre = 0;
    } else {
        LQR_u = angle_control + gyro_control + distance_control + speed_control;
    }
    if (F_ABS(LQR_u) < 5 && wlrobot.joyy == WLR_Off && F_ABS(distance_control) < 4 && (wlrobot.jump_flag == WLR_Off)) {
        LQR_u = PID_PosController(pidSet->lqr_u, LQR_u);
        float lpf_zeropoint_value = Filter_LpfControl(lpfSet->zeropoint, distance_control);
        angle_zeropoint -= PID_PosController(pidSet->zeropoint, lpf_zeropoint_value);
    } else {
        pidSet->lqr_u->error_pre = 0;
    }

    if (wlrobot.height < 50) {
        pidSet->speed->Kp = 0.7;
    } else if (wlrobot.height < 64) {
        pidSet->speed->Kp = 0.6;
    } else {
        pidSet->speed->Kp = 0.5;
    }

    // LOG_INFO("%f,%f,%f,%f", LQR_angle, LQR_gyro, angle_control, gyro_control);
    return LQR_u;
}

// float YAW_gyro = 0; //TODO(oujiali)处理yaw转向
// float YAW_angle = 0;
// float YAW_angle_last = 0;
// float YAW_angle_total = 0;
// float YAW_angle_zero_point = -10;
// float YAW_output = 0;

static float Control_WheelGetYaw(void)
{
    return 0;
}
extern int  g_speed;
static void Control_WheelSetFoc(float target_L, float target_R)
{
    float      shaftL = g_ASdataL.angle_pre;
    float      shaftR = g_ASdataR.angle_pre;
    FilterSet *lpfset = &g_lpfSet;

    shaftL = Filter_LpfControl(lpfset->ang_shaftL, shaftL);
    shaftR = Filter_LpfControl(lpfset->ang_shaftR, shaftR);
    // FOC_WheelBalance(&motor_L, (float)g_speed, shaftL);
    FOC_WheelBalance(&motor_L, target_L, shaftL);
    FOC_WheelBalance(&motor_R, target_R, shaftR);
}

static void Control_WheelBalance(void)
{
    float LQR_u = Control_WheelGetLQR();
    float YAW_output = Control_WheelGetYaw();
    float target_L = -(LQR_u + YAW_output);
    float target_R = -(LQR_u - YAW_output);
    Control_WheelSetFoc(target_L, target_R);
}

void Control_MotionMove(void)
{
    static uint8_t leggel_cnt = 0;
    if (g_flagFocDate == WLR_Act && g_flagMpuDate == WLR_Act) {
        // TODO!(oujiali)进行FOC的平衡控制
        // 1. 更新磁编码器、陀螺仪数据
        // 2. 设置相位、电压，输出foc
        // 3. 计算平衡的pid数据
        Control_WheelBalance();
        if (leggel_cnt >= 100) {
            Control_LeggedBalance();
            leggel_cnt = 0;
        }
        g_flagFocDate = WLR_Idle;
        g_flagMpuDate = WLR_Idle;
        leggel_cnt++;
    }
}
