#include "usart1.h"

void usart1_init(u32 bound)
{
	USART_InitTypeDef	USART_InitStructure;
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 
	
	gpio_init_pin(GPIOA,GPIO_Pin_9 | GPIO_Pin_10,GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_UP);

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE); 
	
	/*
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_InitTypeDef	NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
	*/
}
	
void usart1_send_byte(u8 data)
{
	while((USART1->SR&0X40)==0);
	USART1->DR = data;      
}

void usart1_send_buff(u8 *p,u32 n)
{
	for(;n>0;n--)
	{
		usart1_send_byte(*p);
		p++;
	}
}