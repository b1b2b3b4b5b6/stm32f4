/*
 * init.c
 *
 * Created: 2015/10/6 17:07:32
 *  Author: God
 */

#include "init.h"
#include "CharacterConvert.h"

#define PUTCHAR_PROTOTYPE int putchar (int c)
PUTCHAR_PROTOTYPE
{
	static unsigned char string_GBK[300];
	static unsigned char string_UNICODE[300];
	static unsigned char string_UTF8[400];
	static u8 n=0;
	OS_ERR err;
	usart1_send_byte((u8)c);
	if(c=='\n')
	{
		string_GBK[n]=c;
		string_GBK[n+1]=0;
		gbk_to_unicode_str(string_GBK,string_UNICODE);
		unicode_to_utf8_str(string_UNICODE,string_UTF8);
		OS_TaskQPost(&EmwindemoTaskTCB,string_UTF8,0,OS_OPT_POST_FIFO,0,&err);
		n=0;
	}
	else 
	{
		string_GBK[n]=c;
		n++;
		if(n>=1000) n=0;
	}
	return (c);
}


void clock_init(void)
{
	SystemInit();
/*	 RCC_DeInit();		//时钟初始化
	RCC_HSEConfig(RCC_HSE_ON);		//开HSE，8MHZ
	while(!RCC_WaitForHSEStartUp());		//等待HSE稳定
	RCC_PLLConfig(RCC_PLLSource_HSE,4,168,2,7);		//168MHZ
	 RCC_PLLCmd(ENABLE);		//开PLL
	 while(RCC_GetITStatus(RCC_FLAG_PLLRDY)!=RESET);		//等待PLL就绪
	 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);		//选择PLL为系统时钟

	 RCC_HCLKConfig(RCC_SYSCLK_Div1);				//AHB不分频
	RCC_PCLK1Config(RCC_HCLK_Div4);		//APB1四分频 42MHZ
	RCC_PCLK2Config(RCC_HCLK_Div2);		//APB2二分频 84MHz*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//开启CRC时钟
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//滴答选择外部时钟  HCLK/8

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	//开启GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	//开启GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	//开启GPIOC时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);	//开启GPIOD钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);	//开启GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);	//开启GPIOF时钟

	gpio_init_pin(GPIOA,GPIO_Pin_15,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);
	gpio_init_pin(GPIOB,GPIO_Pin_3,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);
	gpio_init_pin(GPIOB,GPIO_Pin_4,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);	//只开启SW调试io口，A13，A14

	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);		//开启FSMC
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI,ENABLE);		//使能DCMI时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);		//DMA1时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);		//DMA2时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);		//开启SPI1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);		//开启SPI1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);	//开启SDIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	//使能SYSCFG时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);//映射访问CODE区域的内存、选择以太网的什么接口，管理外部中断线到GPIO的连接
	EXTI_DeInit();		//外部中断初始化
	INTX_ENABLE();
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



