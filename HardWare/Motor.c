#include "stm32f10x.h"
#include "PWM.h"

void Motor_Init(void)
{
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 开启GPIOA的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // 开启TIM2时钟

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    PWM_Init(); // 初始化PWM模块
}

void Right_Motor(int8_t Speed) // 右电机
{
    if (Speed >= 0) // 正转
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4); // 控制方向
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        PWM_SetCompare2(Speed); // 设置PWM输出速度
    }
    else if (Speed < 0) // 反转
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_4); // 控制方向
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        PWM_SetCompare2(-Speed); // 设置PWM输出速度
    }
}

void Left_Motor(int8_t Speed) // 左电机
{
    if (Speed >= 0) // 正转
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_6); // 控制方向
        GPIO_SetBits(GPIOA, GPIO_Pin_7);
        PWM_SetCompare3(Speed); // 设置PWM输出速度
    }
    else if (Speed < 0) // 反转
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_6); // 控制方向
        GPIO_ResetBits(GPIOA, GPIO_Pin_7);
        PWM_SetCompare3(-Speed); // 设置PWM输出速度
    }
}
