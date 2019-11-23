#include "stm8l15x.h"

/*******************************************************************************
**函数名称：void delay(unsigned int ms)     
**功能描述：大概延时[非精确延时]
**入口参数：unsigned int ms
**输出：无
*******************************************************************************/
void delay(unsigned int ms)
{
  unsigned int x , y;
  for(x = ms; x > 0; x--)           /*  通过一定周期循环进行延时*/
    for(y = 1000 ; y > 0 ; y--);
}

/*******************************************************************************
**函数名称：void ClockSwitch_HSE()
**功能描述：将系统时钟切换到外部高速晶振时钟
**入口参数：无
**输出：无
*******************************************************************************/
void ClockSwitch_HSE(void)
{
  //启用外部高速晶振且16分频，外部晶振1-16MHz
  
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSE);       //系统主时钟为HSE
  CLK_HSEConfig(CLK_HSE_ON);          //使能HSE起振
  while(CLK_GetFlagStatus(CLK_FLAG_HSERDY)== RESET);  //等待HSE准备就绪
  CLK_SYSCLKSourceSwitchCmd(ENABLE);  //使能时钟切换
  while(CLK_GetFlagStatus(CLK_FLAG_SWBSY) != RESET);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);   //如果切换成功，则设置为1分频，让外部16M时钟作为系统时钟
  
  CLK_SYSCLKSourceSwitchCmd(DISABLE);     //关闭切换
}

void ClockSwitch_HSI(void)
{
  //启用外部高速晶振且16分频，外部晶振1-16MHz

  CLK_SYSCLKSourceSwitchCmd(ENABLE);  //使能时钟切换
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);       //系统主时钟为HSE
  while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)== RESET);  //等待HSE准备就绪
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);   //如果切换成功，则设置为1分频，让外部16M时钟作为系统时钟
  CLK_SYSCLKSourceSwitchCmd(DISABLE);     //关闭切换
}

