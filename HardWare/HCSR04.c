#include "stm32f10x.h"
#include "Delay.h"
#include "Motor.h"
#include "Servo.h"
#include "Car.h"

#define SAFE_LEN 25				// 安全距离阈值(cm)
#define WARNING_LEN 10			// 碰撞危险距离
#define TURN_DELAY 800			// 转向保持时间(ms)
#define BACKUP_DELAY 1000		// 后退持续时间
#define SERVO_STANDBY_DELAY 600 // 舵机稳定时间
// 函数定义
static void Obstacle_Avoidance(void);		 // 避障处理
static void Emergency(void);				 // 撞墙紧急避让
static uint16_t GetDistance(uint16_t angle); // 获得与障碍物距离
static void TurnLeft(void);					 // 小车左转
static void TurnRight(void);				 // 小车右转

void HCSR04_Init()
{
	// 开启GPIOB的时钟和TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 输出模式为推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); // 用来发送10us的高电平到trig

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 输入模式为下拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); // 用来接收超声波测距模块发送到主机的信号

	TIM_InternalClockConfig(TIM1);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 60000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
}
float LENGTH(void)
{
	float Len = 0;
	float Last_Len = 0; // 存储上一次成功测量的距离,超时则返回
	float NUM;
	// 定义超时阈值
	const uint32_t ECHO_WAIT = 30000; // 30ms超时

	GPIO_ResetBits(GPIOB, GPIO_Pin_14); // 先拉低trig,确保发送脉冲前处于低电平

	GPIO_SetBits(GPIOB, GPIO_Pin_14);	// 拉高trig,开始发送高电平脉冲
	Delay_us(40);						// 发送大于20us的脉冲
	GPIO_ResetBits(GPIOB, GPIO_Pin_14); // 拉低电平,发送完毕

	// 等待回波信号,echo收到信号前一直处于高电平
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == RESET)
	{
		if (TIM1->CNT == ECHO_WAIT)
		{
			TIM_Cmd(TIM1, DISABLE);
			return Last_Len; // 超时返回历史值
		}
	}
	TIM1->CNT = 0;
	TIM_Cmd(TIM1, ENABLE);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == SET)
	{
		if (TIM1->CNT == ECHO_WAIT)
		{
			TIM_Cmd(TIM1, DISABLE);
			return Last_Len; // 超时返回历史值
		}
	}
	TIM_Cmd(TIM1, DISABLE);
	NUM = TIM_GetCounter(TIM1);
	Len = (NUM * 1.0 * 0.034) / 2;
	TIM1->CNT = 0;
	Delay_ms(100);
	// 更新历史值
	if (Len > 0)
	{
		Last_Len = Len;
	}
	return Len;
}

void Avoid()
{
	HCSR04_Init();
	Servo_Init();
	Car_Init();

	Goahead();
	uint16_t first_Len = GetDistance(90);

	if (first_Len > WARNING_LEN && first_Len <= SAFE_LEN)
	{
		Obstacle_Avoidance();
	}
	else if (first_Len <= WARNING_LEN)
	{
		Emergency();
	}
}

// 避障处理
static void Obstacle_Avoidance(void)
{
	Stop();
	uint16_t LeftDist = GetDistance(180); // 检测左侧距离
	Delay_ms(SERVO_STANDBY_DELAY);
	uint16_t RightDist = GetDistance(0); // 检测右侧距离

	if (LeftDist > SAFE_LEN && LeftDist > RightDist)
	{
		TurnLeft();
		return;
	}
	else if (RightDist > SAFE_LEN && RightDist > LeftDist)
	{
		TurnRight();
		return;
	}
	// 两侧不通,前方或左右不通执行紧急后退
	Emergency();
}

// 紧急后退处理
static void Emergency()
{
	Stop();
	Back();
	Delay_ms(BACKUP_DELAY);
	Stop();

	// 重新检测环境
	uint16_t Second_LeftDist = GetDistance(180);
	Delay_ms(SERVO_STANDBY_DELAY);
	uint16_t Second_RightDist = GetDistance(0);
	if (Second_LeftDist > SAFE_LEN)
	{
		TurnLeft();
	}
	else if (Second_RightDist > SAFE_LEN)
	{
		TurnRight();
	}
	else if (Second_LeftDist <= WARNING_LEN && Second_RightDist <= WARNING_LEN)
	{
		Back(); // 两侧均危险，继续后退
		Delay_ms(BACKUP_DELAY);
	}
	else
	{
		Goahead();
	}
}

// 距离获取
static uint16_t GetDistance(uint16_t angle)
{
	Set_Servo(angle);
	Delay_ms(SERVO_STANDBY_DELAY);
	uint16_t dist = LENGTH(); // 精确转换为整数
	Set_Servo(90);			  // 复位舵机
	return dist;
}

// 左转
static void TurnLeft(void)
{
	Left();
	Delay_ms(TURN_DELAY);
	Goahead();
}

// 右转
static void TurnRight(void)
{
	Right();
	Delay_ms(TURN_DELAY);
	Goahead();
}
