#include "Camera_usart.h"
#include "Usart.h"

#ifdef CAMERA_USART

/**
 * @Brief: UART1发送数据 UART1 sends data
 * @Note:
 * @Parm: ch:待发送的数据  Data to be sent
 * @Retval:
 */
void USART1_Send_U8(uint8_t ch)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART1, ch);
}

/**
 * @Brief: UART1发送数据 UART1 sends data
 * @Note:
 * @Parm: BufferPtr:待发送的数据 Data to be sent  Length:数据长度 Data length
 * @Retval:
 */
void USART1_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length)
{
	while (Length--)
	{
		USART1_Send_U8(*BufferPtr);
		BufferPtr++;
	}
}

// 串口中断服务函数 Serial port interrupt service function
void USART1_IRQHandler(void)
{
	uint8_t Rx1_Temp = 0;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Rx1_Temp = USART_ReceiveData(USART1);
		USART1_Send_U8(Rx1_Temp);
	}
}

// 发送一个字符 Send a character
void USART2_Send_U8(uint8_t ch)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART2, ch);
}

// 发送一个字符串 Send a string
/**
 * @Brief: UsART2发送数据 UsART2 sends data
 * @Note:
 * @Parm: BufferPtr:待发送的数据  Length:数据长度 BufferPtr: data to be sent Length: data length
 * @Retval:
 */
void USART2_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length)
{
	while (Length--)
	{
		USART2_Send_U8(*BufferPtr);
		BufferPtr++;
	}
}

// 串口中断服务函数 Serial port interrupt service function
void USART2_IRQHandler(void)
{
	uint8_t Rx2_Temp;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Rx2_Temp = USART_ReceiveData(USART2);
		Data_Deal(Rx2_Temp);
		// USART1_Send_U8(Rx2_Temp); //通过串口1打印 Printing through serial port 1
	}
}
#endif // DEBUG
