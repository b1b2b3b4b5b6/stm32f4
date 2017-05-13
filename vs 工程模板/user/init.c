/*
 * init.c
 *
 * Created: 2015/10/6 17:07:32
 *  Author: God
 */ 

#include "init.h"

void clock_init(void)
{
	 RCC_DeInit();		//时钟初始化
	 RCC_HSEConfig(RCC_HSE_ON);		//开HSE，8MHZ
	 while(!RCC_WaitForHSEStartUp());		//等待HSE稳定
	 RCC_PLLConfig(RCC_PLLSource_HSE,4,168,2,4);		//168MHZ
	 RCC_PLLCmd(ENABLE);		//开PLL
	 while(RCC_GetITStatus(RCC_FLAG_PLLRDY)!=RESET);		//等待PLL就绪
	 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);		//选择PLL为系统时钟
	 
	 RCC_HCLKConfig(RCC_SYSCLK_Div1);				//AHB不分频
	 RCC_PCLK1Config(RCC_HCLK_Div4);		//APB1四分频 42MHZ
	 RCC_PCLK2Config(RCC_HCLK_Div2);		//APB2二分频 84MHz
	 
	 SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//滴答选择外部时钟  HCLK/8
	 
																					
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	//开启GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	 //开启GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	//开启GPIOC时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);	 //开启GPIOD钟	 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);	//开启GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);	 //开启GPIOF时钟
	
	gpio_init_pin(GPIOA,GPIO_Pin_15,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);
	gpio_init_pin(GPIOB,GPIO_Pin_3,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);	
	gpio_init_pin(GPIOB,GPIO_Pin_4,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);			//只开启SW调试io口，A13，A14	 
	
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);		//开启FSMC
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);			//开启SPI1
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);			//开启SDIO
	 
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	 
	 EXTI_DeInit();
}

void delay_us(u32 nus)
{
	u32 temp;
	SysTick->LOAD=nus*21; 					//时间加载
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0x00;      					 //清空计数器
}

void delay_ms(u32 nms)
{
	for(;nms>0;nms--)
	{
		delay_us(1000);
	}
}
void gpio_init_pin(GPIO_TypeDef* GPIOx, u16 GPIO_Pin,GPIOMode_TypeDef MODE,GPIOOType_TypeDef OType,GPIOPuPd_TypeDef PUPDR)
{
	GPIO_InitTypeDef io;
	io.GPIO_Mode = MODE;
	io.GPIO_Pin=GPIO_Pin;
	io.GPIO_OType=OType;
	io.GPIO_PuPd=PUPDR;
	io.GPIO_Speed=GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOx,&io);
	
}

void int_to_uchar(int *in,uchar *out,uchar n)
{
	uchar i;
	for (i=0;i<n;i++)
	{
		*out=*in>>8;
		out++;
		*out=*in;
		in++;
		out++;
	}
}

void uchar_to_int(uchar *in,int *out,uchar n)
{
	uchar i;
	for (i=0;i<n;i++)
	{
		*out=*in;
		*out<<=8;
		in++;
		*out|=*in;
		out++;
		in++;
	}
}
