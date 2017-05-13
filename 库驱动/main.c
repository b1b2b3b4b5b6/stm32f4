#include "ili9325.h"
#include "w25qxx.h"
#include "led.h"
#include "sdio_sdcard.h"
#include "exfuns.h"
#include "demo.h"
#include "adc_sample.h"


void fsmc_init(void);
void xpt2046_exit_int(void);

#define START_TASK_PRIO		3	//任务优先级
#define START_STK_SIZE 		512 	//任务堆栈大小
OS_TCB StartTaskTCB;	//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];	//任务堆栈
void start_task(void *p_arg);

#define TOUCH_TASK_PRIO				4
#define TOUCH_STK_SIZE 				1024
OS_TCB TouchTaskTCB;
CPU_STK TOUCH_TASK_STK[START_STK_SIZE];
void touch_task(void *p_arg);

#define EMWINDEMO_TASK_PRIO			6
#define EMWINDEMO_STK_SIZE			2014
OS_TCB EmwindemoTaskTCB;
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
void emwindemo_task(void *p_arg);

#define LED1_TASK_PRIO		5
#define LED1_STK_SIZE 		200
OS_TCB Led1TaskTCB;
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_task(void *p_arg);

vu16 coordinate[2];
vu8 res;
int main(void)

{
	clock_init();
	delay_init(168);  	//时钟初始化

	led_init();
	fsmc_init();
	ili9325_init();
	LED_L(OFF);
	LED_R(OFF);
	ili9325_refresh565(0xffff);
	mem_init();			//初始化内存池
	exfuns_init();		//为fatfs相关变量申请内存
	SD_Init();
	f_mount(fs[0],"0:",1); 					//挂载SD卡
	ili9325_refresh565(0);
	ili9325_horizontal();

	Tim2Config();
	Adc1Config();	//初始化会进入中断，让ADC->SR->VOR置1
	Tim4Config();
	Tim12Config();	//初始化会进入中断
	

	GUI_Init();
	OS_ERR err;
	CPU_SR_ALLOC();
	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区
	OSStart(&err);  //开启UCOSIII


  while(1)
  {
  }
}
//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);
#endif

	OS_CRITICAL_ENTER();	//进入临界区

OSTaskCreate((OS_TCB 	* )&Led1TaskTCB,
				 (CPU_CHAR	* )"led1 task",
                 (OS_TASK_PTR )led1_task,
                 (void		* )0,
                 (OS_PRIO	  )LED1_TASK_PRIO,
                 (CPU_STK   * )&LED1_TASK_STK[0],
                 (CPU_STK_SIZE)LED1_STK_SIZE/10,
                 (CPU_STK_SIZE)LED1_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	//STemWin Demo
	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,
				 (CPU_CHAR*   )"Emwindemo task",
                 (OS_TASK_PTR )emwindemo_task,
                 (void*       )0,
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void*       )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);

	/*OSTaskCreate((OS_TCB*     )&TouchTaskTCB,
				 (CPU_CHAR*   )"Touch task",
                 (OS_TASK_PTR )touch_task,
                 (void*       )0,
                 (OS_PRIO	  )TOUCH_TASK_PRIO,
                 (CPU_STK*    )&TOUCH_TASK_STK[0],
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void*       )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
*/
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务
	OS_CRITICAL_EXIT();	//退出临界区

}



//led1任务函数
void led1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED_R(ON);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
		LED_R(OFF);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
	}
}
//EMWINDEMO
void emwindemo_task(void *p_arg)
{
  	OS_ERR err;
	GUI_SetBkColor(GUI_BLUE);
	MainTask();
	OSTaskDel(&EmwindemoTaskTCB,&err);
}
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);
	}
}

void fsmc_init(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  	FSMC_NORSRAMTimingInitTypeDef  p;

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);	//PD14,FSMC_D0
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);	//PD15,FSMC_D1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);		//PD0,FSMC_D2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);		//PD1,FSMC_D3
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);		//PE7,FSMC_D4
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);		//PE8,FSMC_D5
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);		//PE9,FSMC_D6
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);	//PE10,FSMC_D7
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);	//PE11,FSMC_D8
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);	//PE12,FSMC_D9
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);	//PE13,FSMC_D10
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);	//PE14,FSMC_D11
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);	//PE15,FSMC_D12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);		//PD8,FSMC_D13
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);		//PD9,FSMC_D114
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);	//PD10,FSMC_D15


	gpio_init_pin(GPIOD,GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10,GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_UP);

	gpio_init_pin(GPIOE,GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15,GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_UP);

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);	//FSMC_NOE
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);	//FSMC_NWE
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);	//PD7,FSMC_NE1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC);//PD13,FSMC_A18
	gpio_init_pin(GPIOD,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_13,GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_UP);


  	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  	p.FSMC_AddressSetupTime = 0;
  	p.FSMC_AddressHoldTime = 0;
  	p.FSMC_DataSetupTime = 6;
  	p.FSMC_BusTurnAroundDuration = 0;
  	p.FSMC_CLKDivision = 0;
  	p.FSMC_DataLatency = 0;
  	p.FSMC_AccessMode = FSMC_AccessMode_A;

  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
  	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

