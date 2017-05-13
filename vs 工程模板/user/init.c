/*
 * init.c
 *
 * Created: 2015/10/6 17:07:32
 *  Author: God
 */ 

#include "init.h"

void clock_init(void)
{
	 RCC_DeInit();		//ʱ�ӳ�ʼ��
	 RCC_HSEConfig(RCC_HSE_ON);		//��HSE��8MHZ
	 while(!RCC_WaitForHSEStartUp());		//�ȴ�HSE�ȶ�
	 RCC_PLLConfig(RCC_PLLSource_HSE,4,168,2,4);		//168MHZ
	 RCC_PLLCmd(ENABLE);		//��PLL
	 while(RCC_GetITStatus(RCC_FLAG_PLLRDY)!=RESET);		//�ȴ�PLL����
	 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);		//ѡ��PLLΪϵͳʱ��
	 
	 RCC_HCLKConfig(RCC_SYSCLK_Div1);				//AHB����Ƶ
	 RCC_PCLK1Config(RCC_HCLK_Div4);		//APB1�ķ�Ƶ 42MHZ
	 RCC_PCLK2Config(RCC_HCLK_Div2);		//APB2����Ƶ 84MHz
	 
	 SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//�δ�ѡ���ⲿʱ��  HCLK/8
	 
																					
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	//����GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	 //����GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	//����GPIOCʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);	 //����GPIOD��	 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);	//����GPIOEʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);	 //����GPIOFʱ��
	
	gpio_init_pin(GPIOA,GPIO_Pin_15,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);
	gpio_init_pin(GPIOB,GPIO_Pin_3,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);	
	gpio_init_pin(GPIOB,GPIO_Pin_4,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);			//ֻ����SW����io�ڣ�A13��A14	 
	
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);		//����FSMC
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);			//����SPI1
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//����USART1
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);			//����SDIO
	 
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	 
	 EXTI_DeInit();
}

void delay_us(u32 nus)
{
	u32 temp;
	SysTick->LOAD=nus*21; 					//ʱ�����
	SysTick->VAL=0x00;        					//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL =0x00;      					 //��ռ�����
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
