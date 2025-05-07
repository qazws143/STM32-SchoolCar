#include "stm32f10x.h"
#include "Motor.h"
#include "Car.h"
#include "Delay.h"

// 定义电机速度常量（根据实际测试调整）
#define BASE_SPEED 60
#define TURN_SPEED 50
#define SPIN_SPEED 70

void LinkFllow(void);

void LinkFllow_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Get_Link(int *L1, int *L2, int *R1, int *R2) // 指针接收四路循迹模块的电平
{
  *L1 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
  *L2 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
  *R1 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);
  *R2 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
}

void LinkFllow(void)
{
  // 使用静态变量保持转向标志的状态
  static int flag_left = 0;
  static int flag_right = 0;

  int L1 = 1, L2 = 1, R1 = 1, R2 = 1;
  Get_Link(&L1, &L2, &R1, &R2);
  // 十字路口
  if (L1 == 0 && L2 == 0 && R1 == 0 && R2 == 1)
  {
    if (L1 == 1 && L2 == 0 && R1 == 0 && R2 == 0)
    {
      Left_Motor(-SPIN_SPEED);
      Right_Motor(SPIN_SPEED);
      flag_left = flag_right = 0;
    }
  }
  // 左直角转弯
  else if (L1 == 0 && L2 == 0 && R1 == 0 && R2 == 1)
  {
    Left_Motor(-SPIN_SPEED);
    Right_Motor(SPIN_SPEED);
    flag_left = 1; // 标记左转状态
  }
  // 右直角转弯
  else if (L1 == 1 && L2 == 0 && R1 == 0 && R2 == 0)
  {
    Left_Motor(SPIN_SPEED);
    Right_Motor(-SPIN_SPEED);
    flag_right = 1; // 标记右转状态
  }
  // 左锐角
  else if ((L1 == 0 && L2 == 0 && R1 == 0) || (L1 == 0 && L2 == 1 && R1 == 0))
  {
    Left_Motor(-SPIN_SPEED);
    Right_Motor(SPIN_SPEED);
  }

  // 右锐角
  else if ((L2 == 0 && R1 == 0 && R2 == 0) || (L2 == 0 && R1 == 1 && R2 == 0))
  {
    Left_Motor(SPIN_SPEED);
    Right_Motor(-SPIN_SPEED);
  }
  //  左大弯
  else if ((L1 == 0 || L2 == 0) && R2 == 0)
  {
    Left_Motor(-TURN_SPEED);
    Right_Motor(BASE_SPEED);
    flag_left = 1;
  }
  // 右大弯
  else if ((R1 == 0 || R2 == 0) && L1 == 0)
  {
    Left_Motor(BASE_SPEED);
    Right_Motor(-TURN_SPEED);
    flag_right = 1;
  }
  // 左小弯
  else if (L2 == 0 && R1 == 1)
  {
    Left_Motor(-TURN_SPEED);
    Right_Motor(BASE_SPEED);
    flag_left = 1;
  }
  // 右小弯
  else if (R1 == 0 && L2 == 1)
  {
    Left_Motor(BASE_SPEED);
    Right_Motor(-TURN_SPEED);
    flag_right = 1;
  }
  // 直行
  else if (L2 == 0 && R1 == 0)
  {
    Left_Motor(BASE_SPEED);
    Right_Motor(BASE_SPEED);
    flag_left = flag_right = 0;
  }
  // 向右微调
  else if (flag_right)
  {
    Left_Motor(TURN_SPEED);
    Right_Motor(-TURN_SPEED);
  }
  else if (flag_left)
  {
    Left_Motor(-TURN_SPEED);
    Right_Motor(TURN_SPEED);
  }
}
