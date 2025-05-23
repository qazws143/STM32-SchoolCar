#include "stm32f10x.h"

void Beep_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}

void Beep_On(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_8);
}

void Beep_Off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}