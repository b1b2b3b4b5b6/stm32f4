#include "spi2.h"

void spi2_init(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	SPI_StructInit(&SPI_InitStruct);
	SPI_InitStruct.SPI_Mode=SPI_Mode_Master;
	SPI_InitStruct.SPI_BaudRatePrescaler =SPI2_SPEED;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;		//管理:内部NSS信号由SSI位控制
	SPI_Init(SPIX2, &SPI_InitStruct);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd(SPIX2, ENABLE); //使能SPI外设
	spi2_gpio_init();
}

void spi2_gpio_init()
{
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF_SPI1); //PB3复用为 SPI2
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI1); //PB4复用为 SPI2
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_SPI1); //PB5复用为 SPI2
		gpio_init_pin(GPIOC,GPIO_Pin_2|GPIO_Pin_3,GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_UP);
		gpio_init_pin(GPIOB,GPIO_Pin_10,GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_UP);
}

u16 spi2_rw(u16 byte)
{
	SPI_I2S_SendData(SPIX2, byte);
	while(!SPI_I2S_GetFlagStatus(SPIX2,SPI_I2S_FLAG_RXNE));
	byte=SPI_I2S_ReceiveData(SPIX2);
	return byte;
}

uchar spi2_write_reg(uchar reg, uchar value)
{
	uchar status;
	status = spi2_rw(reg);
	spi2_rw(value);
	return	status;
}



uchar spi2_read_reg(uchar reg)
{
	uchar value;
	spi2_rw(reg);
	value=spi2_rw(0xFF);
	return value;
}

uchar spi2_write_buf(uchar reg,uchar *Buf,u32 n)
{
	uchar status;
	status=spi2_rw(reg);
	for(;n>0;n--)
	{
		spi2_rw(*Buf);
		Buf++;
	}
	return status;
}

uchar spi2_read_buf(uchar reg,uchar *Buf,u32 n)
{
	uchar status;
	status=spi2_rw(reg);
	for(;n>0;n--)
	{
		*Buf=spi2_rw(0);
		Buf++;
	}
	return status;
}

void spi2_speed(u32 SPI_BaudRatePrescaler)
{
	 assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	 SPI_Cmd(SPIX2, DISABLE); //禁止SPI外设
	 SPIX2->CR1=(SPIX2->CR1&~0x0038)|SPI_BaudRatePrescaler;
	 SPI_Cmd(SPIX2, ENABLE); //使能SPI外设
}