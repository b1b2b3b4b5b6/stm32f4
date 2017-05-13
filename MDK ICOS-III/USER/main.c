

#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include	"enc28j60.h"
#include "sys.h"
#include "delay.h"
#include "usart1.h"
#include "includes.h"
#include "init.h"

extern unsigned long lwip_localtime;
void TIM3_Int_Init(u16 arr,u16 psc);
#define START_TASK_PRIO		3	//?????
#define START_STK_SIZE 		512 	//??????
OS_TCB StartTaskTCB;	//?????
CPU_STK START_TASK_STK[START_STK_SIZE];	//????
void start_task(void *p_arg);


#define LWIP_TASK_PRIO			4
#define LWIP_STK_SIZE			4000
OS_TCB	LwipTaskTCB;
CPU_STK LWIP_TASK_STK[LWIP_STK_SIZE];
void Lwip_task(void *p_arg);



int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	clock_init();
	delay_init(168);  //?????

	usart1_init(460800);   //?????
	TIM3_Int_Init(1000,719);
	my_mem_init();			//??????

	OSInit(&err);		    	//???UCOSIII
	OS_CRITICAL_ENTER();	//?????			 
	//??????
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//?????
				 (CPU_CHAR	* )"start task", 		//????
                 (OS_TASK_PTR )start_task, 			//????
                 (void		* )0,					//??????????
                 (OS_PRIO	  )START_TASK_PRIO,     //?????
                 (CPU_STK   * )&START_TASK_STK[0],	//???????
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//????????
                 (CPU_STK_SIZE)START_STK_SIZE,		//??????
                 (OS_MSG_QTY  )0,					//???????????????????,?0???????
                 (OS_TICK	  )0,					//???????????????,?0??????,
                 (void   	* )0,					//????????
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //????
                 (OS_ERR 	* )&err);				//????????????
	OS_CRITICAL_EXIT();	//?????	 
	OSStart(&err);      //??UCOSIII
}


//??????
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//????
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//?????????????
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //???????????
	 //???????????,??????1???????,?1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);
#endif

	OS_CRITICAL_ENTER();	//?????

	  OSTaskCreate((OS_TCB*     )&LwipTaskTCB,
			   (CPU_CHAR*   )"Lwip task",
			   (OS_TASK_PTR )Lwip_task,
			   (void*       )0,
			   (OS_PRIO	  )LWIP_TASK_PRIO,
			   (CPU_STK*    )&LWIP_TASK_STK[0],
			   (CPU_STK_SIZE)LWIP_STK_SIZE/10,
			   (CPU_STK_SIZE)LWIP_STK_SIZE,
			   (OS_MSG_QTY  )0,
			   (OS_TICK	  )0,
			   (void*       )0,
			   (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			   (OS_ERR*     )&err);

	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//??????
	OS_CRITICAL_EXIT();	//?????

}






void Lwip_task(void *p_arg)
{
	printf("LWIP Initing......\r\n");
	while(lwip_comm_init()) 
	{
		printf("LWIP Init Falied!Retrying...\r\n");
	}
	printf("LWIP Init Sucess!\r\n");
#if LWIP_DHCP   
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))
	{
		lwip_periodic_handle();	
	}
#endif
 	while(1)
	{	
		lwip_periodic_handle();	
	}
}
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //κ±?σκ1?ό
	

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  
	
	TIM_Cmd(TIM3, ENABLE);  			 
}


void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //ς?3??D??
	{
		lwip_localtime +=10; //?σ10
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //??3y?D??±κ????
}

