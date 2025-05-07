// 单片机头文件
#include "stm32f10x.h"
#include <string.h>
// 硬件头文件
#include "Delay.h"
#include "USART.h"
#include "dht11.h"
#include "oled.h"
#include "GPS_usart.h"
#include "HCSR04.h"
#include "LinkFllow.h"
#include "Car.h"
#include "Motor.h"
#include "Camera.h"
#include "mq.h"
#include "beep.h"
// 云平台头文件
#include "onenet.h"
#include "esp8266.h"

#define ESP8266_ONENET_INFO "AT+CIPSTART=\"TCP\",\"mqtt.heclouds.com\",1883\r\n"

u8 temperature = 0, humidity = 0;
float CH4;
extern double f_longitude, f_latitude;

void HardWare_Init(void)
{
    SystemInit();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 中断控制器分组设置
    Delay_Init();
    ADCx_Init();
    OLED_Init();
    Car_Init();
    Motor_Init();
    LinkFllow_Init();
    Beep_Init();
    Usart3_Init(115200); // 串口3，驱动ESP8266用
    uart_init(115200);   // GPS串口初始化
}

void Car(float CH4)
{
    if (CH4 >= 1000)
    {
        Right_Motor(0);
        Left_Motor(0);
        Beep_On();
    }
    else
    {
        Beep_Off();
        Goahead();
    }
}

void GPS(void)
{
    parseGpsBuffer();
    printGpsBuffer(); // 读取经纬度
}

int main(void)
{
    unsigned char *dataPtr = NULL;

    HardWare_Init();

    while (DHT11_Init())
    {
        Delay_ms(200);
    }
    OLED_ShowString(20, 24, "Networking", 16, 1);
    ESP8266_Init(); // 初始化ESP8266
    OLED_ShowString(0, 24, "                ", 16, 1);
    OLED_ShowString(0, 24, " Connected to", 16, 1);
    OLED_ShowString(30, 48, "ONENET", 16, 1);
    while (ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
        Delay_ms(500);
    UsartPrintf(USART_DEBUG, "NET_OK\r\n");

    while (OneNet_DevLink()) // 接入OneNET
        Delay_ms(500);
    OLED_Clear();

    OLED_ShowString(0, 0, "tem", 8, 1);
    OLED_ShowString(0, 12, "hum", 8, 1);
    OLED_ShowString(80, 0, " C", 8, 1);
    OLED_ShowString(80, 12, " %RH", 8, 1);
    OLED_ShowString(0, 24, "CH4:", 8, 1);
    OLED_ShowString(80, 24, " ppm", 8, 1);
    OLED_ShowString(0, 36, "lat:", 8, 1);
    OLED_ShowString(0, 48, "lon:", 8, 1);

    OLED_Refresh();
    // OneNET_Subscribe();
    clrStruct();
    while (1)
    {
        DHT11_Read_Data(&temperature, &humidity); // 读取温湿度
        parseGpsBuffer();
        printGpsBuffer();         // 读取经纬度
        CH4 = mq_get_value_mq4(); // 读取有害气体浓度
        Car(CH4);
        OLED_ShowNum(60, 0, temperature, 2, 8, 1); // 显示温度
        OLED_ShowNum(60, 12, humidity, 2, 8, 1);   // 湿度数值
        OLED_ShowNum(40, 24, CH4, 6, 8, 1);        // 显示CH4浓度
        OneNet_SendData();                         // 发送数据

        ESP8266_Clear();
        dataPtr = ESP8266_GetIPD(0);
        if (dataPtr != NULL)
            OneNet_RevPro(dataPtr);

        Delay_ms(10);
    }
}
