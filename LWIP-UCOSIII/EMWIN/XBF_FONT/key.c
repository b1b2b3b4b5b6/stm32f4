#include "key.h"

OS_SEM KeySem_0;
OS_SEM KeySem_1;

void key_init(void)
{
	OS_ERR err;
	gpio_init_pin(GPIOE,GPIO_Pin_3,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_UP);
	gpio_init_pin(GPIOE,GPIO_Pin_4,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_UP);
	//key_exit_init();
	OSSemCreate(&KeySem_0,0,0,&err);
	OSSemCreate(&KeySem_1,0,0,&err);
}

void key_exit_init()
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef 	EXTI_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;// 抢占优先级为3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;// 子优先级位3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据上面指定的参数初始化NVIC寄存器
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;// 抢占优先级为3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;// 子优先级位3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据上面指定的参数初始化NVIC寄存器
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI3_IRQHandler(void)
{
	OS_ERR err;
	#if OS_SUPPORT 		//如果OS_SUPPORT为真，则需要支持OS.
		OSIntEnter();    
	#endif
		
	OS_SemPost(&KeySem_1,
			   OS_OPT_POST_1,	//一次
			   0,
			   &err);
	EXTI_ClearITPendingBit(EXTI_Line3); //清除中断线3挂起标志位

	#if OS_SUPPORT 	//如果OS_SUPPORT为真，则需要支持OS.
		OSIntExit();  											 
	#endif
}

void EXTI4_IRQHandler(void)
{
	OS_ERR err;
	#if OS_SUPPORT 		//如果OS_SUPPORT为真，则需要支持OS.
		OSIntEnter();    
	#endif
	OS_SemPost(&KeySem_0,
			   OS_OPT_POST_1,	//一次
			   0,
			   &err);
	EXTI_ClearITPendingBit(EXTI_Line4); //清除中断线3挂起标志位

	#if OS_SUPPORT 	//如果OS_SUPPORT为真，则需要支持OS.
		OSIntExit();  											 
	#endif
}

void key_scan(void)
{
	OS_ERR err;
	if(KEY_0==0)  OS_SemPost(&KeySem_0,OS_OPT_POST_1,0,&err);
	if(KEY_1==0)  OS_SemPost(&KeySem_1,OS_OPT_POST_1,0,&err);
}

//OSSemPend(&KeySem_0,0,OS_OPT_PEND_BLOCKING,&ts,&err);