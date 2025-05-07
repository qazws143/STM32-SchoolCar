#ifndef __ESP32_WIFI_H
#define __ESP32_WIFI_H

#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "Camera_usart.h"
#include "Delay.h"

// wifi模式配置 如果都关闭，默认使用MODE_AP_STA模式 Wi-Fi
#define MODE_AP 0     // 0关闭 1开启 0 Off 1 On
#define MODE_STA 0    // 0关闭 1开启 0 Off 1 On
#define MODE_AP_STA 1 // 0关闭 1开启 0 Off 1 On

typedef struct QR_AI_Msg_t
{
    char QR_msg[50]; // QRmsg的处理 QRmsg processing
} QR_AI_Msg;

void SET_ESP_WIFI_MODE(void);
void SET_STA_WIFI(void);
void SET_AP_WIFI(void);

void Get_STAIP(void);
void Get_APIP(void);
void Get_Version(void);

void Data_Deal(uint8_t RXdata);
void IP_identify(uint8_t *data);
void recv_tcp_data(char tcpdata);
extern uint8_t newlines;

#endif
