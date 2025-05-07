#include "mq.h"
#include "Delay.h"
#include "adc.h"
#include <math.h> // ���� pow ����

#define FILTER_SIZE 10 // �˲����ڴ�С

// �õ�ADC�����ڲ���������ֵ
// ȡ10��,Ȼ��ƽ��
uint32_t mq_get_value_adc_mq4()
{
    uint32_t val = 0;
    uint8_t count;
    for (count = 0; count < 10; count++) {
        val = ADC_ConvertedValue[0] + val; // ��ȡDMAͨ��ֵ
        Delay_ms(3);
    }
    return val / 10;
}

// ���ص�ѹֵ
float mq_get_value_vrl_mq4(void)
{
    uint32_t val = 0;
    val          = mq_get_value_adc_mq4();
    return (float)val / 4096 * 5; // ��ADCֵת��Ϊ��ѹ
}

// MQ2���������ݴ��������˲������ƹ��ܣ�
float mq_get_value_mq4(void)
{
    static float filter_buffer[FILTER_SIZE] = {0}; // �˲�������
    static uint8_t filter_index             = 0;   // �˲�����������
    float vrl                               = mq_get_value_vrl_mq4();

    // ����ppmֵ
    float ppm = pow((3.4880 * 10 * vrl) / (5 - vrl), (1.0 / 0.3203));
    ppm       = ppm / 100; // ת��Ϊ�ٷֱ�

    // ����ǰֵ�����˲�������
    filter_buffer[filter_index] = ppm;
    filter_index                = (filter_index + 1) % FILTER_SIZE;

    // �����˲����ƽ��ֵ
    float filtered_ppm = 0;
    for (uint8_t i = 0; i < FILTER_SIZE; i++) {
        filtered_ppm += filter_buffer[i];
    }
    filtered_ppm /= FILTER_SIZE;

    // �������ֵ�������쳣ֵ
    if (filtered_ppm > 100000) // �������ֵΪ10000 ppm
    {
        filtered_ppm = 100000;
    }

    return filtered_ppm;
}
