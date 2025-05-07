#include "sys.h"
#include "stdlib.h"
#include "GPS_usart.h"
#include "oled.h"
char rxdatabufer;
u16 point1 = 0;

double f_latitude = 0.0;
double f_longitude = 0.0;

_SaveData Save_Data;

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);

    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {
    }

    return ch;
}
int GetKey(void)
{

    while (!(USART1->SR & USART_FLAG_RXNE))
        ;

    return ((int)(USART1->DR & 0x1FF));
}

#if EN_USART1_RX // 如果使能了接收
// 串口1中断服务程序
// 注意,读取USARTx->SR能避免莫名其妙的错误
char USART_RX_BUF[USART_REC_LEN]; // 接收缓冲,最大USART_REC_LEN个字节.
// 接收状态
// bit15，	接收完成标志
// bit14，	接收到0x0d
// bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0; // 接收状态标记

void uart_init(u32 bound)
{
    // GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); // 使能USART1，GPIOA时钟
                                                                                  // USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                                     // PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    //     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           // 根据指定的参数初始化VIC寄存器

    // USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;                                     // 一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式

    USART_Init(USART1, &USART_InitStructure);      // 初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启中断
    USART_Cmd(USART1, ENABLE);                     // 使能串口

    CLR_Buf(); // 清空缓存
}

void USART1_IRQHandler(void) // 串口1中断服务程序
{
    u8 Res;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        Res = USART_ReceiveData(USART1); //(USART1->DR);	//读取接收到的数据

        if (Res == '$')
        {
            point1 = 0;
        }

        USART_RX_BUF[point1++] = Res;

        if (USART_RX_BUF[0] == '$' && USART_RX_BUF[4] == 'M' && USART_RX_BUF[5] == 'C') // 确定是否收到"GPRMC/GNRMC"这一帧数据
        {
            if (Res == '\n')
            {
                memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length); // 清空
                memcpy(Save_Data.GPS_Buffer, USART_RX_BUF, point1); // 保存数据
                Save_Data.isGetData = true;
                point1 = 0;
                memset(USART_RX_BUF, 0, USART_REC_LEN); // 清空
            }
        }

        if (point1 >= USART_REC_LEN)
        {
            point1 = USART_REC_LEN;
        }
    }
}

u8 Hand(char *a) // 串口命令识别函数
{
    if (strstr(USART_RX_BUF, a) != NULL)
        return 1;
    else
        return 0;
}

void CLR_Buf(void) // 串口缓存清理
{
    memset(USART_RX_BUF, 0, USART_REC_LEN); // 清空
    point1 = 0;
}

void clrStruct()
{
    Save_Data.isGetData = false;
    Save_Data.isParseData = false;
    Save_Data.isUsefull = false;
    memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length); // 清空
    memset(Save_Data.UTCTime, 0, UTCTime_Length);
    memset(Save_Data.latitude, 0, latitude_Length);
    memset(Save_Data.N_S, 0, N_S_Length);
    memset(Save_Data.longitude, 0, longitude_Length);
    memset(Save_Data.E_W, 0, E_W_Length);
}
#endif

void errorLog(int num)
{

    while (1)
    {
        printf("ERROR%d\r\n", num);
    }
}

void parseGpsBuffer()
{
    char *subString;
    char *subStringNext;
    char i = 0;
    if (Save_Data.isGetData)
    {
        Save_Data.isGetData = false;
        printf("**************\r\n");
        printf(Save_Data.GPS_Buffer);

        for (i = 0; i <= 6; i++)
        {
            if (i == 0)
            {
                if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
                    errorLog(1); // 解析错误
            }
            else
            {
                subString++;
                if ((subStringNext = strstr(subString, ",")) != NULL)
                {
                    char usefullBuffer[2];
                    switch (i)
                    {
                    case 1:
                        memcpy(Save_Data.UTCTime, subString, subStringNext - subString);
                        break; // 获取UTC时间
                    case 2:
                        memcpy(usefullBuffer, subString, subStringNext - subString);
                        break; // 获取UTC时间
                    case 3:
                        memcpy(Save_Data.latitude, subString, subStringNext - subString);
                        break; // 获取纬度信息
                    case 4:
                        memcpy(Save_Data.N_S, subString, subStringNext - subString);
                        break; // 获取N/S
                    case 5:
                        memcpy(Save_Data.longitude, subString, subStringNext - subString);
                        break; // 获取经度信息
                    case 6:
                        memcpy(Save_Data.E_W, subString, subStringNext - subString);
                        break; // 获取E/W

                    default:
                        break;
                    }

                    subString = subStringNext;
                    Save_Data.isParseData = true;
                    if (usefullBuffer[0] == 'A')
                        Save_Data.isUsefull = true;
                    else if (usefullBuffer[0] == 'V')
                        Save_Data.isUsefull = false;
                }
                else
                {
                    errorLog(2); // 解析错误
                }
            }
        }
    }
}

// GPS数据转化单位为度。
double Convert_to_degrees(char *data)
{
    double temp_data = atof(data);
    int degree = (int)(temp_data / 100);
    double f_degree = (temp_data / 100.0 - degree) * 100 / 60.0;
    double result = degree + f_degree;
    return result;
}

void printGpsBuffer()
{
    if (Save_Data.isParseData)
    {
        Save_Data.isParseData = false;
        printf("Save_Data.UTCTime = ");
        printf(Save_Data.UTCTime);
        printf("\r\n");

        if (Save_Data.isUsefull)
        {
            Save_Data.isUsefull = false;
            printf("Save_Data.latitude = ");

            f_latitude = Convert_to_degrees(Save_Data.latitude);
            printf("%lf%s", f_latitude, Save_Data.N_S);
            OLED_ShowNum(40, 36, f_latitude, 5, 8, 1); // 显示纬度
            printf("\r\n");

            printf("Save_Data.N_S = ");
            printf(Save_Data.N_S);
            printf("\r\n");

            printf("Save_Data.longitude = ");

            f_longitude = Convert_to_degrees(Save_Data.longitude);
            printf("%lf%s", f_longitude, Save_Data.E_W);
            OLED_ShowNum(40, 48, f_longitude, 5, 8, 1); // 显示经度
            printf("\r\n");

            printf("Save_Data.E_W = ");
            printf(Save_Data.E_W);
            printf("\r\n");
        }
        else
        {
            printf("GPS DATA is not usefull!\r\n");
        }
    }
}
