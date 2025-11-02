#include "order.h"

#include <stdlib.h>
#include "info.h"
#include "esp8266.h"

static uint32_t Order_CheckCharFloat(uint8_t *pos, uint8_t len)
{
    RET_IF(len <= 0 || len >= RX_BUF_SIZE, WLR_ERROR);
    uint8_t *point_pos = (uint8_t *)strstr((char *)pos, ".");

    for (uint16_t i = 0; i < len; i++) {
        if ((pos[i] < '0' || pos[i] > '9') && (point_pos != &pos[i])) {
            LOG_ERROR("check char can not convert to float");
            return WLR_ERR65537;
        }
    }
    return WLR_OK;
}

static void Oreder_SetValue(uint8_t *pos, uint8_t len)
{
    RET_IF(len <= 0 || len >= RX_BUF_SIZE);
    char S = *pos++;
    len--;

    RET_IF(Order_CheckCharFloat(pos, len) != WLR_OK);
    char cmd[RX_BUF_SIZE] = {0};
    memcpy(cmd, pos, len);
    cmd[len] = '\0';
    double num = atof(cmd);
    LOG_INFO("set value: %f", num);
    PIDSet    *pidSet = Info_GetPidSet();
    MpuData_t *MPUdata = Info_GetMpuData();
    if (S == 'A') {
        pidSet->angle->Kp = num;
    } else if (S == 'D') {
        pidSet->gyro->Kp = num;
    } else if (S == 'Z') {
        MPUdata->angleY_zeropoint = num;
    }
}

void Order_ParseCommand(void)
{
    if (g_flagUart1Recv == WLR_Act) {
        Command_t *command = Info_GetUsartCommand();
        uint8_t   *start_pos = command->buff;
        uint8_t    cmd_len = command->size;
        if (g_flagUart1Prefix != WLR_Off) {
            start_pos = (uint8_t *)strstr((char *)command->buff, ":");
            RET_IF(start_pos == NULL);
            start_pos++;
            RET_IF(start_pos == NULL);
            cmd_len = command->size - (uint8_t)(command->buff - start_pos);
            RET_IF(cmd_len <= 0);
        }
        HAL_UART_Transmit(&huart1, start_pos, cmd_len, 1000);
        switch (*start_pos) {
        case 'S':
            Oreder_SetValue(start_pos + 1, cmd_len - 1);
            break;

        default:
            LOG_WARN("vailed command");
            break;
        }
        g_flagUart1Recv = WLR_Idle;
    }
}

void Order_Init(void)
{
    Usart_StartRecvByIdle();
    ESP8266_Init(ESP8266_Sta, ESP8266_Client);
}