#include <stdio.h>
#include "stm8l15x.h"
/*******************************************************************************
**函数名称：void UART1_Init(u32 baudrate)
**功能描述：初始化USART模块
**入口参数：u32 baudrate  -> 设置串口波特率
**输出：无
*******************************************************************************/
void UART1_Init(u32 baudrate)
{   
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , ENABLE); //使能USART1时钟
  USART_Init(USART1,                //设置USART1
             baudrate,               //波特率设置
             USART_WordLength_8b,    //数据长度设为8位
             USART_StopBits_1,       //1位停止位
             USART_Parity_No,        //无校验
             (USART_Mode_Rx | USART_Mode_Tx));  //设置为发送接收双模式
  
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);  //使能接收中断
  USART_Cmd(USART1 , ENABLE);       //使能USART1开始工作
}
/*******************************************************************************
**函数名称：void Uart_SendData(u8  data)
**功能描述：向串口发送寄存器写入一个字节数据
**入口参数：u8  data
**输出：无
*******************************************************************************/
void Uart_SendData(u8  data)
{
  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == 0);        //判断发送数据寄存器是否为空   
  USART_SendData8(USART1 , (u8)data);                     //向发送寄存器写入数据      
}

/*******************************************************************************
**函数名称：int fputc(int ch, FILE *f)
**功能描述：是Printf库函数的中间连接函数
**入口参数：
**输出：无
*******************************************************************************/
int fputc(int ch, FILE *f)
{      
  Uart_SendData((u8)(ch));     
  return ch;
}