#include "stm32f10x.h"

void PWM_Servo_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // ����Ϊ�����������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  TIM_InternalClockConfig(TIM3); // ����TIM3�ڲ�ʱ��
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
  TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); // ��ʼ��TIM3ʱ����Ԫ

  /*����Ƚϳ�ʼ��*/
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCStructInit(&TIM_OCInitStructure);                       // �ṹ���ʼ��
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // ���PWMģʽ1
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // ����Ϊ��
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // ʹ�����
  TIM_OCInitStructure.TIM_Pulse = 0;

  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  /*TIMʹ��*/
  TIM_Cmd(TIM3, ENABLE); // ʹ��TIM3����ʼ��ʱ��
}

void PWM_SetCompare4(uint16_t Compare)
{
  TIM_SetCompare4(TIM3, Compare);
}
