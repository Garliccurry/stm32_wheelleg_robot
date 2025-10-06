#include "esp8266.h"

static void ESP8266_HardwareReset(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_Delay(500);
}

static uint32_t ESP8266_SendATCmd(char *cmd, char *ack, uint32_t timeout)
{
    RET_IF(cmd == NULL, WLR_ERROR);
    ESP8266usart_Reset();
    ESP8266usart_Printf("%s\r\n", cmd);

    uint32_t ret = WLR_OK;
    if (ack == NULL || timeout == 0) {
        return WLR_OK;
    } else {
        while (timeout > 0) {
            ret = ESP8266usart_GetRecvData();
            if (ret == WLR_OK) {
                if (strstr((const char *)g_command.buff, ack) != NULL) {
                    return WLR_OK;
                } else {
                    ESP8266usart_Reset();
                }
            }
            timeout--;
            HAL_Delay(1);
        }
        return WLR_ERROR;
    }
}

uint32_t ESP8266_Restore(void)
{
    uint32_t ret = ESP8266_SendATCmd("AT+RESTORE", NULL, 0);
    if (ret == WLR_OK) {
        HAL_Delay(3000);
        return WLR_OK;
    }
    return ret;
}

uint32_t ESP8266_ATTest(void)
{
    uint32_t ret = WLR_OK;
    for (uint8_t i = 0; i < 10; i++) {
        ret = ESP8266_SendATCmd("AT", "OK", 500);
        RET_IF(ret == WLR_OK, WLR_OK);
    }
    return ret;
}

uint32_t ESP8266_SetMode(ESP8266_WifiMode mode)
{
    if (mode > 0 && mode <= 3) {
        char cmd[64] = {0};
        sprintf(cmd, "AT+CWMODE=%d", mode);
        return ESP8266_SendATCmd(cmd, "OK", 500);
    }
    return WLR_ERROR;
}

uint32_t ESP8266_SoftwareReset(void)
{
    uint32_t ret = WLR_OK;
    ret = ESP8266_SendATCmd("AT+RST", NULL, 0);
    if (ret == WLR_OK) {
        HAL_Delay(3000);
        return WLR_OK;
    }
    return ret;
}

uint32_t ESP8266_AteConfig(uint8_t cfg)
{
    if (cfg == 0 || cfg == 1) {
        char cmd[64] = {0};
        sprintf(cmd, "ATE%d", cfg);
        return ESP8266_SendATCmd(cmd, "OK", 500);
    }
    return WLR_ERROR;
}

uint32_t ESP8266_JoinAp(char *ssid, char *pwd)
{
    char cmd[64] = {0};
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    return ESP8266_SendATCmd(cmd, "OK", 10000);
}

uint32_t ESP8266_QuitAp(void)
{
    return ESP8266_SendATCmd("AT+CWQAP", "OK", 10000);
}

uint32_t ESP8266_SetupAp(char *ssid, char *pwd, uint8_t chl, uint8_t ecn)
{
    char cmd[64] = {0};
    sprintf(cmd, "AT+CWSAP=\"%s\",\"%s\",%u,%u", ssid, pwd, chl, ecn);
    return ESP8266_SendATCmd(cmd, "OK", 10000);
}

uint32_t ESP8266_GetIp(char *buf)
{
    uint32_t ret = WLR_OK;
    char    *p_start = NULL;
    char    *p_end = NULL;

    ret = ESP8266_SendATCmd("AT+CIFSR", "OK", 500);
    RET_IF(ret != WLR_OK, ret);
    p_start = strstr((const char *)g_command.buff, "\"");
    p_end = strstr(p_start + 1, "\"");
    *p_end = '\0';
    sprintf(buf, "%s", p_start + 1);
    return WLR_OK;
}

uint32_t ESP8266_SetTcpMux(uint8_t mux)
{
    char cmd[64] = {0};
    sprintf(cmd, "AT+CIPMUX=%u", mux);
    return ESP8266_SendATCmd(cmd, "OK", 1000);
}

uint32_t ESP8266_ConnectTcpServer(char *server_ip, char *server_port)
{
    char cmd[64] = {0};
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", server_ip, server_port);
    return ESP8266_SendATCmd(cmd, "OK", 5000);
}

uint32_t ESP8266_CreatTcpServer(char *server_port)
{
    char cmd[64] = {0};
    sprintf(cmd, "AT+CIPSERVER=1,\"%s\"", server_port);
    return ESP8266_SendATCmd(cmd, "OK", 5000);
}

uint32_t ESP8266_DisconnectTcpServer(ESP8266_Endpoint role)
{
    if (role == ESP8266_Client) {
        return ESP8266_SendATCmd("AT+CIPCLOSE", "OK", 5000);
    } else {
        return ESP8266_SendATCmd("AT+CIPCLOSE=5", "OK", 5000);
    }
}

uint32_t ESP8266_EnterUnvarnished(void)
{
    uint32_t ret = WLR_OK;
    ret = ESP8266_SendATCmd("AT+CIPMODE=1", "OK", 500);
    RET_IF(ret != WLR_OK, ret);
    ret = ESP8266_SendATCmd("AT+CIPSEND", ">", 500);
    RET_IF(ret != WLR_OK, ret);
    return WLR_OK;
}

void ESP8266_ExitUnvarnished(void)
{
    ESP8266usart_Printf("+++");
    HAL_Delay(1000);
}

uint32_t ESP8266_ServerPresend(uint8_t size)
{
    char cmd[64] = {0};
    sprintf(cmd, "AT+CIPSEND=0,%u", size);
    return ESP8266_SendATCmd(cmd, ">", 1000);
}

uint32_t ESP8266_ConnectAtkCloud(char *id, char *pwd)
{
    char cmd[64] = {0};
    sprintf(cmd, "AT+ATKCLDSTA=\"%s\",\"%s\"", id, pwd);
    return ESP8266_SendATCmd(cmd, "CLOUD CONNECTED", 10000);
}

uint32_t ESP8266_DisconnectAtkCloud(void)
{
    return ESP8266_SendATCmd("AT+ATKCLDCLS", "CLOUD DISCONNECT", 500);
}

void ESP8266_Init(ESP8266_WifiMode mode, ESP8266_Endpoint role)
{
    uint32_t ret = WLR_OK;
    ESP8266usart_Reset();
#ifdef IMU_BOARD_2
    ESP8266_HardwareReset(); // only use for verson2.0 IMU board 仅仅可以在2.0版本后的IMU板子使用
#endif
    do {
        ret = ESP8266_Restore();
        BREAK_IF(ret != WLR_OK);
        ret = ESP8266_ATTest();
        BREAK_IF(ret != WLR_OK);
        ret = ESP8266_SetMode(mode);
        BREAK_IF(ret != WLR_OK);
        ret = ESP8266_SoftwareReset();
        BREAK_IF(ret != WLR_OK);
        ret = ESP8266_AteConfig(ATE0);
        BREAK_IF(ret != WLR_OK);

        if (mode == ESP8266_Sta) {
            ret = ESP8266_JoinAp(PC_WIFI_SSID, PC_WIFI_PWD);
            BREAK_IF(ret != WLR_OK);
        } else {
            ret = ESP8266_SetupAp(ESP_WIFI_SSID, ESP_WIFI_SSID, 1, 3);
            BREAK_IF(ret != WLR_OK);
        }

        if (role == ESP8266_Client) {
            g_flagUart1Prefix = WLR_Off;
            ret = ESP8266_SetTcpMux(0);
            BREAK_IF(ret != WLR_OK);
            ret = ESP8266_ConnectTcpServer(PC_SERVER_IP, PC_SERVER_PORT);
            BREAK_IF(ret != WLR_OK);
            ret = ESP8266_EnterUnvarnished();
            BREAK_IF(ret != WLR_OK);
        } else {
            g_flagUart1Prefix = WLR_On;
            ret = ESP8266_SetTcpMux(1);
            BREAK_IF(ret != WLR_OK);
            ret = ESP8266_CreatTcpServer(ESP_SERVER_PORT);
            BREAK_IF(ret != WLR_OK);
        }

    } while (0);

    if (ret != 0) {
        g_flagFatalErr = WLR_On;
    }
    ESP8266usart_Reset();
}