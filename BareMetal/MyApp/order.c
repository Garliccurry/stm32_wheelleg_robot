#include "order.h"

#include "info.h"

uint8_t gCommand[RX_BUF_SIZE];

int  g_speed = 0;
void Order_ParseCommand(void)
{
    if (g_flagUart1Recv == WLR_Act) {
        // if (gCommand[0] == 0x31) {
        //     pos_left += 6;
        // } else if (gCommand[3] == 0x32) {
        //     g_vel += 10.f;
        //     printf("%f\r\n", g_vel);
        // } else if (gCommand[3] == 0x33) {
        //     g_vel -= 10.f;
        //     printf("%f\r\n", g_vel);
        // } else if (gCommand[3] == 0x34) {
        //     g_hight += 10;
        //     printf("%d\r\n", g_hight);
        // } else if (gCommand[3] == 0x35) {
        //     g_hight -= 10;
        //     printf("%d\r\n", g_hight);
        // } else if (gCommand[3] == 0x36) {
        //     wheel_run = ~wheel_run;
        //     MOTOR_L_TOGGLE;
        //     MOTOR_R_TOGGLE;
        // }

        // switch (gCommand[3]) {
        // case '1':
        //     g_speed += 1;
        //     LOG_DEBUG("INC SPEED +: %f", g_speed);
        //     break;
        // case '2':
        //     g_speed -= 1;
        //     LOG_DEBUG("DEC SPEED -: %f", g_speed);
        //     break;
        // default:
        //     break;
        // }
        HAL_UART_Transmit(&huart1, gCommand, RX_BUF_SIZE, 1000);
        g_flagUart1Recv = WLR_Idle;
    }
}