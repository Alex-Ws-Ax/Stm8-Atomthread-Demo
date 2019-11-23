#include "stm8l15x.h"
#include <stdio.h>
#include "RC522.h"
#include "Beep.h"
#include "System.h"
#include "Uart.h"


/*  ������  */
void main()
{
u8 CardID[32]={0};

  disableInterrupts();  //�ر�ϵͳ���ж�
  ClockSwitch_HSI();
  BeepInit();
  UART1_Init(9600);     //����LED1��LED2��LED3��ˮ�ƶ�ӦIO�ڳ�ʼ��
  CardReaderInit();
  enableInterrupts();   //ʹ��ϵͳ���ж�
  BeepLong();
  while(1)
  {
	if(GetCardID(CardID)==MI_OK)
	{
		Beep();
	}
	delay(1000);
  }
  
}