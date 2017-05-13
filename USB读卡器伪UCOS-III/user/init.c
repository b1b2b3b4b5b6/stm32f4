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
/*	 RCC_DeInit();		//ʱ�ӳ�ʼ��
	RCC_HSEConfig(RCC_HSE_ON);		//��HSE��8MHZ
	while(!RCC_WaitForHSEStartUp());		//�ȴ�HSE�ȶ�
	RCC_PLLConfig(RCC_PLLSource_HSE,4,168,2,7);		//168MHZ
	 RCC_PLLCmd(ENABLE);		//��PLL
	 while(RCC_GetITStatus(RCC_FLAG_PLLRDY)!=RESET);		//�ȴ�PLL����
	 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);		//ѡ��PLLΪϵͳʱ��

	 RCC_HCLKConfig(RCC_SYSCLK_Div1);				//AHB����Ƶ
	RCC_PCLK1Config(RCC_HCLK_Div4);		//APB1�ķ�Ƶ 42MHZ
	RCC_PCLK2Config(RCC_HCLK_Div2);		//APB2����Ƶ 84MHz*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//����CRCʱ��
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//�δ�ѡ���ⲿʱ��  HCLK/8

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	//����GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	//����GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	//����GPIOCʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);	//����GPIOD��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);	//����GPIOEʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);	//����GPIOFʱ��

	gpio_init_pin(GPIOA,GPIO_Pin_15,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);
	gpio_init_pin(GPIOB,GPIO_Pin_3,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);
	gpio_init_pin(GPIOB,GPIO_Pin_4,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_DOWN);	//ֻ����SW����io�ڣ�A13��A14

	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);		//����FSMC
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI,ENABLE);		//ʹ��DCMIʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);		//DMA1ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);		//DMA2ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);		//����SPI1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);		//����SPI1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//����USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);	//����SDIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	//ʹ��SYSCFGʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);//ӳ�����CODE������ڴ桢ѡ����̫����ʲô�ӿڣ������ⲿ�ж��ߵ�GPIO������
	EXTI_DeInit();		//�ⲿ�жϳ�ʼ��
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



