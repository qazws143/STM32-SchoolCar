#include "stm32f10x.h"
#include "Motor.h"

void Car_Init()
{
	Motor_Init();
}

void Goahead()
{
	Right_Motor(50);
	Left_Motor(50);
}

void Back()
{
	Right_Motor(-50);
	Left_Motor(-50);
}

void Stop()
{
	Right_Motor(0);
	Left_Motor(0);
}

void Left() // 左转
{
	Right_Motor(100);
	Left_Motor(-100);
}

void Right() // 右转
{
	Right_Motor(-100);
	Left_Motor(100);
}
