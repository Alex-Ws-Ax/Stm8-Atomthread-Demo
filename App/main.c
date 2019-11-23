#include "stm8l15x.h"
#include <stdio.h>
#include "RC522.h"
#include "Beep.h"
#include "System.h"
#include "Uart.h"


/*  主函数  */
void main()
{
u8 CardID[32]={0};

  disableInterrupts();  //关闭系统总中断
  ClockSwitch_HSI();
  BeepInit();
  UART1_Init(9600);     //调用LED1、LED2、LED3流水灯对应IO口初始化
  CardReaderInit();
  enableInterrupts();   //使能系统总中断
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