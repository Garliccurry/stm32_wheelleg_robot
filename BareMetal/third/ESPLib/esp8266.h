#ifndef _ESP8266_H__
#define _ESP8266_H__
#include "main.h"
#include "esp8266_usart.h"

#define ESP_WIFI_SSID   "WLR_8266"
#define ESP_WIFI_PWD    "12345678"
#define ESP_SERVER_IP   "192.168.4.1"
#define ESP_SERVER_PORT "8080"

#define PC_WIFI_SSID   "Garlic_PC"
#define PC_WIFI_PWD    "12345678"
#define PC_SERVER_IP   "192.168.137.1"
#define PC_SERVER_PORT "8080"

#define PHONE_WIFI_SSID   "Redmi carry"
#define PHONE_WIFI_PWD    "12345679"
#define PHONE_SERVER_IP   "0.0.0.0"
#define PHONE_SERVER_PORT "0000"

#define ATE0 0
#define ATE1 1

#define IMU_BOARD_1

typedef enum {
    ESP8266_Sta = 1,
    ESP8266_AP,
    ESP8266_StaAp
} ESP8266_WifiMode;

typedef enum {
    ESP8266_Client = 0,
    ESP8266_Server = 1
} ESP8266_Endpoint;

uint32_t ESP8266_ServerPresend(uint8_t size);
void     ESP8266_Init(ESP8266_WifiMode mode, ESP8266_Endpoint role);
#endif