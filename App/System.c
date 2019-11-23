#include "stm8l15x.h"

/*******************************************************************************
**�������ƣ�void delay(unsigned int ms)     
**���������������ʱ[�Ǿ�ȷ��ʱ]
**��ڲ�����unsigned int ms
**�������
*******************************************************************************/
void delay(unsigned int ms)
{
  unsigned int x , y;
  for(x = ms; x > 0; x--)           /*  ͨ��һ������ѭ��������ʱ*/
    for(y = 1000 ; y > 0 ; y--);
}

/*******************************************************************************
**�������ƣ�void ClockSwitch_HSE()
**������������ϵͳʱ���л����ⲿ���پ���ʱ��
**��ڲ�������
**�������
*******************************************************************************/
void ClockSwitch_HSE(void)
{
  //�����ⲿ���پ�����16��Ƶ���ⲿ����1-16MHz
  
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSE);       //ϵͳ��ʱ��ΪHSE
  CLK_HSEConfig(CLK_HSE_ON);          //ʹ��HSE����
  while(CLK_GetFlagStatus(CLK_FLAG_HSERDY)== RESET);  //�ȴ�HSE׼������
  CLK_SYSCLKSourceSwitchCmd(ENABLE);  //ʹ��ʱ���л�
  while(CLK_GetFlagStatus(CLK_FLAG_SWBSY) != RESET);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);   //����л��ɹ���������Ϊ1��Ƶ�����ⲿ16Mʱ����Ϊϵͳʱ��
  
  CLK_SYSCLKSourceSwitchCmd(DISABLE);     //�ر��л�
}

void ClockSwitch_HSI(void)
{
  //�����ⲿ���پ�����16��Ƶ���ⲿ����1-16MHz

  CLK_SYSCLKSourceSwitchCmd(ENABLE);  //ʹ��ʱ���л�
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);       //ϵͳ��ʱ��ΪHSE
  while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)== RESET);  //�ȴ�HSE׼������
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);   //����л��ɹ���������Ϊ1��Ƶ�����ⲿ16Mʱ����Ϊϵͳʱ��
  CLK_SYSCLKSourceSwitchCmd(DISABLE);     //�ر��л�
}

