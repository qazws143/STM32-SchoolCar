#include "stm32f10x.h"
#include "PWM_Servo.h"

void Servo_Init()
{
	PWM_Servo_Init();
}

void Set_Servo(float Angle)
{
	PWM_SetCompare4(Angle / 180 * 2000 + 500);
}
