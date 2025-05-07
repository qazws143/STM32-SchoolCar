#include "Camera.h"
#include "Camera_usart.h"

#if MODE_AP_STA
#define WIFI_MODE '2'

#elif MODE_STA
#define WIFI_MODE '1'

#elif MODE_AP
#define WIFI_MODE '0'

#else
#define WIFI_MODE '2'
#endif

#define STAIP "sta_ip"
#define STA_WIFI_SSID "Feng"   // wifi名称
#define STA_WIFI_PD "88888888" // wifi密码

#define APIP "ap_ip"
#define AP_WIFI_SSID "ESP32_WIFI_TEST" // wifi名称
#define AP_WIFI_PD ""                  // wifi密码

uint8_t send_buf[35] = {0};  // 发送命令的
uint8_t recv_buf[50] = {0};  // 接收
uint8_t data_buff[50] = {0}; // 备份

uint8_t cmd_flag = 0; // 发送命令标志 0:未发送 1:已发送 2:网络透传

#ifdef CAMERA
// 设置STA模式WiFi
void SET_STA_WIFI(void)
{
    // 发送SSID
    sprintf((char *)send_buf, "sta_ssid:%s", STA_WIFI_SSID);
    USART2_Send_ArrayU8(send_buf, strlen((char *)send_buf));
    memset(send_buf, 0, sizeof(send_buf));
    Delay_ms(300);

    // 发送密码
    sprintf((char *)send_buf, "sta_pd:%s", STA_WIFI_PD);
    USART2_Send_ArrayU8(send_buf, strlen((char *)send_buf));
    memset(send_buf, 0, sizeof(send_buf));

    Delay_ms(2); // 等待复位重启
}

// 设置AP模式
void SET_AP_WIFI(void)
{
    // 发送SSID
    sprintf((char *)send_buf, "ap_ssid:%s", AP_WIFI_SSID);
    USART2_Send_ArrayU8(send_buf, strlen((char *)send_buf));
    memset(send_buf, 0, sizeof(send_buf));
    Delay_ms(300);

    // 发送密码
    sprintf((char *)send_buf, "ap_pd:%s", AP_WIFI_PD);
    USART2_Send_ArrayU8(send_buf, strlen((char *)send_buf));
    memset(send_buf, 0, sizeof(send_buf));

    Delay_ms(2); // 等待复位重启
}

void SET_ESP_WIFI_MODE(void) // 设置WiFi模式
{
    // 选择STA+AP共存模式
    sprintf((char *)send_buf, "wifi_mode:%c", WIFI_MODE);
    USART2_Send_ArrayU8(send_buf, strlen((char *)send_buf));
    memset(send_buf, 0, sizeof(send_buf));

    Delay_ms(2); // 等待复位重启
}

// 查询STA模式IP
void Get_STAIP(void)
{
    sprintf((char *)send_buf, STAIP);
    USART2_Send_ArrayU8(send_buf, strlen((char *)send_buf));
    memset(send_buf, 0, sizeof(send_buf));
    cmd_flag = 1;
}

// 查询AP模式IP
void Get_APIP(void)
{
    sprintf((char *)send_buf, APIP);
    USART2_Send_ArrayU8(send_buf, strlen((char *)send_buf));
    memset(send_buf, 0, sizeof(send_buf));
    cmd_flag = 1;
}

void Get_Version(void)
{
    // 查询固件版本
    sprintf((char *)send_buf, "wifi_ver");
    USART2_Send_ArrayU8(send_buf, strlen((char *)send_buf));
    memset(send_buf, 0, sizeof(send_buf));
    cmd_flag = 1;
}

//-----------------------------------数据处理----------------------------------
uint8_t end_falg = 0;
uint8_t i_index = 0;

void Data_Deal(uint8_t RXdata)
{
    if (cmd_flag == 1)
    {
        recv_buf[i_index] = RXdata;

        if (RXdata == 0x0D)
        {
            end_falg = 1;
        }

        if (end_falg == 1 && RXdata == 0x0A)
        {
            cmd_flag = 0;
            end_falg = 0;
            memcpy(data_buff, recv_buf, i_index);
            memset(recv_buf, 0, sizeof(recv_buf));
            i_index = 0;
        }
        else
            i_index++;
    }

    // 网络透传数据处理
    else if (cmd_flag == 2)
    {
        recv_tcp_data(RXdata);
    }
}

// IP信息处理
void IP_identify(uint8_t *data)
{
    if (memcmp(data, "sta_ip", 6) == 0)
    {
        printf("%s", data);
    }
    else if (memcmp(data, "ap_ip", 5) == 0)
    {
        printf("%s", data);
    }
    else if (memcmp(data, "VerSion", 7) == 0)
    {
        printf("%s", data);
    }
    memset(data_buff, 0, sizeof(data_buff));
}

// 网络透传数据处理
uint8_t g_new_flag = 0;
uint8_t g_index = 0;

void recv_tcp_data(char tcpdata)
{
    if (tcpdata == '$' && g_new_flag == 0)
    {
        g_new_flag = 1;
        memset(recv_buf, 0, sizeof(recv_buf));
        return;
    }
    if (g_new_flag == 1)
    {
        if (tcpdata == '#')
        {
            g_new_flag = 0;
            g_index = 0;
            memcpy(data_buff, recv_buf, sizeof(recv_buf));
            memset(recv_buf, 0, sizeof(recv_buf));
        }
        else if (tcpdata == '$')
        {
            g_index = 0;
            g_new_flag = 0;
            memset(recv_buf, 0, sizeof(recv_buf));
        }
        else
        {
            recv_buf[g_index++] = tcpdata;
        }

        if (g_index > 50)
        {
            g_index = 0;
            g_new_flag = 0;
            memset(recv_buf, 0, sizeof(recv_buf));
        }
    }
}

int Camera()
{
    SystemInit();

    USART1_init(115200); // 连接PC
    USART2_init(115200); // 连接摄像头模块

    Delay_ms(2); // 等待摄像头启动

    printf("Initializing WiFi...\r\n");
    SET_ESP_WIFI_MODE();
    SET_STA_WIFI();
    SET_AP_WIFI();

    Get_APIP();
    Delay_ms(2);
    IP_identify(data_buff);

    Get_STAIP();
    Delay_ms(2);
    IP_identify(data_buff);

    cmd_flag = 2; // 进入透传模式
    Delay_ms(100);

    while (1)
    {
        // 主循环保持运行
    }
}
#endif