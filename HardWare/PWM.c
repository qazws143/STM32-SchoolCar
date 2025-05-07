#include "stm32f10x.h"

void PWM_Init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // 开启TIM2的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启GPIOA的时钟

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // 将PA2和PA1引脚初始化为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_1; // 同时初始化PA2和PA1
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*时基单元初始化*/
    TIM_InternalClockConfig(TIM2); // 选择TIM2为内部时钟

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;   // 周期为100个时钟周期
    TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1; // 预分频器为36
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure); // 初始化TIM2时基单元

    /*输出比较初始化*/
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);                       // 结构体初始化
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // 输出PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // 极性为高
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;                            // 初始值为0

    TIM_OC3Init(TIM2, &TIM_OCInitStructure); // 配置通道3 (PA2)
    TIM_OC2Init(TIM2, &TIM_OCInitStructure); // 配置通道2 (PA1)

    /*TIM使能*/
    TIM_Cmd(TIM2, ENABLE); // 使能TIM2，开始定时器
}

void PWM_SetCompare3(uint16_t Compare)
{
    TIM_SetCompare3(TIM2, Compare); // 设置CCR3的值
}

void PWM_SetCompare2(uint16_t Compare)
{
    TIM_SetCompare2(TIM2, Compare);
}
