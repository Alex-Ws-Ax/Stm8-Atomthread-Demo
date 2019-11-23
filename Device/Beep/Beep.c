#include "stm8l15x.h"
#include "stm8l15x_beep.h"
extern void delay(unsigned int ms);

void BeepInit(void)
{
	CLK_BEEPClockConfig(CLK_BEEPCLKSource_LSI);
	CLK_PeripheralClockConfig(CLK_Peripheral_BEEP , ENABLE);
	GPIO_Init(GPIOA,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Fast);//CS
	BEEP_DeInit();
	BEEP_Init(BEEP_Frequency_1KHz);
	
}
void Beep(void)
{
	BEEP_Cmd(ENABLE);
	delay(100);
	BEEP_Cmd(DISABLE);
}
void BeepLong(void)
{
	BEEP_Cmd(ENABLE);
	delay(500);
	BEEP_Cmd(DISABLE);
}
