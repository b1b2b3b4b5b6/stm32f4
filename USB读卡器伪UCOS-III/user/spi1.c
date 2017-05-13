/*
 * spi.c
 *
 * Created: 2015/10/25 0:03:38
 *  Author: God
 */ 
#include "spi1.h"



void spi1_init(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	SPI_StructInit(&SPI_InitStruct);
	SPI_InitStruct.SPI_Mode=SPI_Mode_Master;
	SPI_InitStruct.SPI_BaudRatePrescaler =SPI1_SPEED;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;		//管理:内部NSS信号由SSI位控制
	SPI_Init(SPIX1, &SPI_InitStruct);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd(SPIX1, ENABLE); //使能SPI外设
	spi1_gpio_init();
}

void spi1_gpio_init()
{
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1); //PB3复用为 SPI1
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB4复用为 SPI1
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1); //PB5复用为 SPI1
		gpio_init_pin(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5,GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_UP);
}

u16 spi1_rw(u16 byte)
{
	SPI_I2S_SendData(SPIX1, byte);
	while(!SPI_I2S_GetFlagStatus(SPIX1,SPI_I2S_FLAG_RXNE));
	byte=SPI_I2S_ReceiveData(SPIX1);
	return byte;
}

uchar spi1_write_reg(uchar reg, uchar value)
{
	uchar status;
	status = spi1_rw(reg);
	spi1_rw(value);
	return	status;
}



uchar spi1_read_reg(uchar reg)
{
	uchar value;
	spi1_rw(reg);
	value=spi1_rw(0xFF);
	return value;
}

uchar spi1_write_buf(uchar reg,uchar *Buf,u32 n)
{
	uchar status;
	status=spi1_rw(reg);
	for(;n>0;n--)
	{
		spi1_rw(*Buf);
		Buf++;
	}
	return status;
}

uchar spi1_read_buf(uchar reg,uchar *Buf,u32 n)
{
	uchar status;
	status=spi1_rw(reg);
	for(;n>0;n--)
	{
		*Buf=spi1_rw(0);
		Buf++;
	}
	return status;
}

void spi1_speed(u32 SPI_BaudRatePrescaler)
{
	 assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	 SPI_Cmd(SPIX1, DISABLE); //禁止SPI外设
	 SPIX1->CR1=(SPIX1->CR1&~0x0038)|SPI_BaudRatePrescaler;
	 SPI_Cmd(SPIX1, ENABLE); //使能SPI外设
}
