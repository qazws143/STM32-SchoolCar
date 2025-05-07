#include "stm32f10x.h"

void PWM_Servo_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 设置为复用推挽输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  TIM_InternalClockConfig(TIM3); // 开启TIM3内部时钟
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
  TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); // 初始化TIM3时基单元

  /*输出比较初始化*/
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCStructInit(&TIM_OCInitStructure);                       // 结构体初始化
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // 输出PWM模式1
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // 极性为高
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
  TIM_OCInitStructure.TIM_Pulse = 0;

  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  /*TIM使能*/
  TIM_Cmd(TIM3, ENABLE); // 使能TIM3，开始定时器
}

void PWM_SetCompare4(uint16_t Compare)
{
  TIM_SetCompare4(TIM3, Compare);
}
