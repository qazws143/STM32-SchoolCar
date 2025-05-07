#include "mq.h"
#include "Delay.h"
#include "adc.h"
#include <math.h> // 用于 pow 函数

#define FILTER_SIZE 10 // 滤波窗口大小

// 得到ADC采样内部传感器的值
// 取10次,然后平均
uint32_t mq_get_value_adc_mq4()
{
    uint32_t val = 0;
    uint8_t count;
    for (count = 0; count < 10; count++) {
        val = ADC_ConvertedValue[0] + val; // 获取DMA通道值
        Delay_ms(3);
    }
    return val / 10;
}

// 返回电压值
float mq_get_value_vrl_mq4(void)
{
    uint32_t val = 0;
    val          = mq_get_value_adc_mq4();
    return (float)val / 4096 * 5; // 将ADC值转换为电压
}

// MQ2传感器数据处理（增加滤波和限制功能）
float mq_get_value_mq4(void)
{
    static float filter_buffer[FILTER_SIZE] = {0}; // 滤波缓冲区
    static uint8_t filter_index             = 0;   // 滤波缓冲区索引
    float vrl                               = mq_get_value_vrl_mq4();

    // 计算ppm值
    float ppm = pow((3.4880 * 10 * vrl) / (5 - vrl), (1.0 / 0.3203));
    ppm       = ppm / 100; // 转换为百分比

    // 将当前值存入滤波缓冲区
    filter_buffer[filter_index] = ppm;
    filter_index                = (filter_index + 1) % FILTER_SIZE;

    // 计算滤波后的平均值
    float filtered_ppm = 0;
    for (uint8_t i = 0; i < FILTER_SIZE; i++) {
        filtered_ppm += filter_buffer[i];
    }
    filtered_ppm /= FILTER_SIZE;

    // 限制最大值，避免异常值
    if (filtered_ppm > 100000) // 假设最大值为10000 ppm
    {
        filtered_ppm = 100000;
    }

    return filtered_ppm;
}
