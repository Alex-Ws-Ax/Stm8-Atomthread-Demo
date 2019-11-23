
#include "stm8l15x_spi.h"
#include "Spi.h"
void SpiPortInit(void)
{
	GPIO_Init(GPIOB,GPIO_Pin_4,GPIO_Mode_Out_PP_Low_Slow);//CS
	GPIO_Init(GPIOB,GPIO_Pin_5,GPIO_Mode_Out_PP_Low_Slow);//SCK
	GPIO_Init(GPIOB,GPIO_Pin_6,GPIO_Mode_Out_PP_Low_Slow);//MOSI
	GPIO_Init(GPIOB,GPIO_Pin_7,GPIO_Mode_In_PU_No_IT);//MISO
}
void SpiCSLow(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
}
void SpiCSHigh(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
}
void SpiInit(void)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE); 
	SYSCFG_REMAPPinConfig(REMAP_Pin_SPI1Full,DISABLE);
	SPI_DeInit(SPI1);
	SpiPortInit();
	SPI_Init(SPI1,SPI_FirstBit_MSB,\
		SPI_BaudRatePrescaler_2,\
		SPI_Mode_Master,\
		SPI_CPOL_Low,\
		SPI_CPHA_1Edge,\
		SPI_Direction_2Lines_FullDuplex,\
		SPI_NSS_Soft,\
		0x07);
	SPI_Cmd(SPI1,ENABLE);
}
u8 SpiSendRecv(u8 data)
{
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE)==RESET);
	SPI_SendData(SPI1,data);
	while(SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE)==RESET);
	return SPI_ReceiveData(SPI1);
}
