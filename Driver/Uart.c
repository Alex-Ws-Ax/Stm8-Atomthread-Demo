#include <stdio.h>
#include "stm8l15x.h"
/*******************************************************************************
**�������ƣ�void UART1_Init(u32 baudrate)
**������������ʼ��USARTģ��
**��ڲ�����u32 baudrate  -> ���ô��ڲ�����
**�������
*******************************************************************************/
void UART1_Init(u32 baudrate)
{   
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , ENABLE); //ʹ��USART1ʱ��
  USART_Init(USART1,                //����USART1
             baudrate,               //����������
             USART_WordLength_8b,    //���ݳ�����Ϊ8λ
             USART_StopBits_1,       //1λֹͣλ
             USART_Parity_No,        //��У��
             (USART_Mode_Rx | USART_Mode_Tx));  //����Ϊ���ͽ���˫ģʽ
  
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);  //ʹ�ܽ����ж�
  USART_Cmd(USART1 , ENABLE);       //ʹ��USART1��ʼ����
}
/*******************************************************************************
**�������ƣ�void Uart_SendData(u8  data)
**�����������򴮿ڷ��ͼĴ���д��һ���ֽ�����
**��ڲ�����u8  data
**�������
*******************************************************************************/
void Uart_SendData(u8  data)
{
  while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == 0);        //�жϷ������ݼĴ����Ƿ�Ϊ��   
  USART_SendData8(USART1 , (u8)data);                     //���ͼĴ���д������      
}

/*******************************************************************************
**�������ƣ�int fputc(int ch, FILE *f)
**������������Printf�⺯�����м����Ӻ���
**��ڲ�����
**�������
*******************************************************************************/
int fputc(int ch, FILE *f)
{      
  Uart_SendData((u8)(ch));     
  return ch;
}