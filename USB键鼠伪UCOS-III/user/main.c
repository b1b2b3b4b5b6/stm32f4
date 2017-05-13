/**************************************************
中断说明：
1:SysTick_Handler
2:PendSV_Handler
3:EXTI2_IRQHandler-------------key0---------3
4:EXTI3_IRQHandler-------------key1---------3
5:EXTI0_IRQHandler-------------ENC28J60-----0
**************************************************/

#include "ili9325.h"
#include "w25qxx.h"
#include "led.h"
#include "demo.h"
#include "adc_sample.h"
#include "enc28j60.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "emwin_spi_font.h"

#include "usbh_usr.h" 

#define START_TASK_PRIO		3	//任务优先级
#define START_STK_SIZE 		128 	//任务堆栈大小
OS_TCB StartTaskTCB;	//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];	//任务堆栈
void start_task(void *p_arg);

#define LED1_TASK_PRIO		4
#define LED1_STK_SIZE 		128
OS_TCB Led1TaskTCB;
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_task(void *p_arg);

#define KEYSCAN_TASK_PRIO				5
#define KEYSCAN_STK_SIZE 				128
OS_TCB KeyScanTaskTCB;
CPU_STK KEYSCAN_TASK_STK[START_STK_SIZE];
void KeyScan_task(void *p_arg);

#define EMWINDEMO_TASK_PRIO			6
#define EMWINDEMO_STK_SIZE			256
OS_TCB	EmwindemoTaskTCB;
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
void emwindemo_task(void *p_arg);

#define LWIP_TASK_PRIO			15				//TCP ----112
#define LWIP_STK_SIZE			256				//pkt-----10
OS_TCB	LwipTaskTCB;							//dhcp----11
CPU_STK LWIP_TASK_STK[LWIP_STK_SIZE];
void Lwip_task(void *p_arg);

#define DEMO_TASK_PRIO			20
#define DEMO_STK_SIZE			256
OS_TCB	DemoTaskTCB;
CPU_STK DEMO_TASK_STK[DEMO_STK_SIZE];
void Demo_task(void *p_arg);

#define KEY0_TASK_PRIO			30
#define KEY0_STK_SIZE			128
OS_TCB	Key0TaskTCB;
CPU_STK KEY0_TASK_STK[KEY0_STK_SIZE];
void Key0_task(void *p_arg);


#define KEY1_TASK_PRIO			30
#define KEY1_STK_SIZE			128
OS_TCB	Key1TaskTCB;
CPU_STK KEY1_TASK_STK[KEY1_STK_SIZE];
void Key1_task(void *p_arg);

#define USB_TASK_PRIO			10
#define USB_STK_SIZE			1024
OS_TCB	USBTaskTCB;
CPU_STK USB_TASK_STK[USB_STK_SIZE];
void USB_task(void *p_arg);

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core_dev;
extern HID_Machine_TypeDef HID_Machine;		
extern OS_Q HID_Q;

int main(void)
{   
	clock_init();
	delay_init(168);  	//时钟初始化
	usart1_init(460800);
	ili9325_init();
	W25QXX_Init();
 	led_init();
	key_init();
	ili9325_refresh565(0xffff);
	my_mem_init();			//初始化内存池
	//exfuns_init();		//为fatfs相关变量申请内存
	//SD_Init();
	//f_mount(fs[0],"0:",1); 					//挂载SD卡
	ili9325_refresh565(0);
	ili9325_horizontal();
	LED_R(ON);


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


	OSTaskCreate((OS_TCB*     )&KeyScanTaskTCB,
				 (CPU_CHAR*   )"KeyScan task",
                 (OS_TASK_PTR )KeyScan_task,
                 (void*       )0,
                 (OS_PRIO	  )KEYSCAN_TASK_PRIO,
                 (CPU_STK*    )&KEYSCAN_TASK_STK[0],
                 (CPU_STK_SIZE)KEYSCAN_STK_SIZE/10,
                 (CPU_STK_SIZE)KEYSCAN_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void*       )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);

	  OSTaskCreate((OS_TCB*     )&Key0TaskTCB,
			   (CPU_CHAR*   )"Key0 task",
			   (OS_TASK_PTR )Key0_task,
			   (void*       )0,
			   (OS_PRIO	  )KEY0_TASK_PRIO,
			   (CPU_STK*    )&KEY0_TASK_STK[0],
			   (CPU_STK_SIZE)KEY0_STK_SIZE/10,
			   (CPU_STK_SIZE)KEY0_STK_SIZE,
			   (OS_MSG_QTY  )0,
			   (OS_TICK	  )0,
			   (void*       )0,
			   (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			   (OS_ERR*     )&err);

	  OSTaskCreate((OS_TCB*     )&Key1TaskTCB,
			   (CPU_CHAR*   )"Key1 task",
			   (OS_TASK_PTR )Key1_task,
			   (void*       )0,
			   (OS_PRIO	  )KEY1_TASK_PRIO,
			   (CPU_STK*    )&KEY1_TASK_STK[0],
			   (CPU_STK_SIZE)KEY1_STK_SIZE/10,
			   (CPU_STK_SIZE)KEY1_STK_SIZE,
			   (OS_MSG_QTY  )0,
			   (OS_TICK	  )0,
			   (void*       )0,
			   (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			   (OS_ERR*     )&err);
	 /*
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
	  */
	  OSTaskCreate((OS_TCB*     )&DemoTaskTCB,
			   (CPU_CHAR*   )"Demo task",
			   (OS_TASK_PTR )Demo_task,
			   (void*       )0,
			   (OS_PRIO	  )DEMO_TASK_PRIO,
			   (CPU_STK*    )&DEMO_TASK_STK[0],
			   (CPU_STK_SIZE)DEMO_STK_SIZE/10,
			   (CPU_STK_SIZE)DEMO_STK_SIZE,
			   (OS_MSG_QTY  )0,
			   (OS_TICK	  )0,
			   (void*       )0,
			   (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			   (OS_ERR*     )&err);
	
	  OSTaskCreate((OS_TCB*     )&USBTaskTCB,
			   (CPU_CHAR*   )"USB task",
			   (OS_TASK_PTR )USB_task,
			   (void*       )0,
			   (OS_PRIO	  )USB_TASK_PRIO,
			   (CPU_STK*    )&USB_TASK_STK[0],
			   (CPU_STK_SIZE)USB_STK_SIZE/10,
			   (CPU_STK_SIZE)USB_STK_SIZE,
			   (OS_MSG_QTY  )0,
			   (OS_TICK	  )0,
			   (void*       )0,
			   (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			   (OS_ERR*     )&err);
	  
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务
	OS_CRITICAL_EXIT();	//退出临界区

}

//led1任务函数
void led1_task(void *p_arg)
{
	OS_ERR err;
	
	while(1)
	{
		LED_R(ON);
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);
		LED_R(OFF);
		//printf("时钟节拍:%d\n",OSTimeGet(&err));
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);
	}
}
//EMWINDEMO
void emwindemo_task(void *p_arg)
{
  	OS_ERR err;
	
	MainTask();
	
	OSTaskDel(&EmwindemoTaskTCB,&err);
}
void KeyScan_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size;
	CPU_TS ts;
	void *data;
	while(1)
	{
		key_scan();
		data=OSQPend(&HID_Q,
				0,
				OS_OPT_PEND_NON_BLOCKING,
				&size,
				&ts,
				&err);
		if(err==OS_ERR_NONE)
		{
			switch(size)
			{
			  case 0:
			  	printf("keyborad:%d\r\n",(uint8_t)data);
			  break;

			  case 1:
				
			  	break;
				
			  default:
				
				break;
			}
		}
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_NON_STRICT,&err);
	}
}

void Lwip_task(void *p_arg)
{
	OS_ERR err;
	printf("LWIP Initing......\r\n");
	while(lwip_comm_init()) 
	{
		printf("LWIP Init Falied!Retrying...\r\n");
	}
	printf("LWIP Init Sucess!\r\n");
	
	OSTaskDel(&LwipTaskTCB,&err);
}

void Demo_task(void *p_arg)
{
	OS_ERR err;

	

	OSTaskDel(&DemoTaskTCB,&err);
}


void Key0_task(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	while(1)
	{
		OSSemPend(&KeySem_0,0,OS_OPT_PEND_BLOCKING,&ts,&err);

		OSSemSet(&KeySem_1,0,&err);
	}
	OSTaskDel(&Key0TaskTCB,&err);
}


void Key1_task(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	while(1)
	{

		OSSemPend(&KeySem_1,0,OS_OPT_PEND_BLOCKING,&ts,&err);

		OSSemSet(&KeySem_1,0,&err);
		
	}
	OSTaskDel(&Key1TaskTCB,&err);
}

void USB_task(void *p_arg)
{

	OS_ERR err;
	
	USBH_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USB_Host,&HID_cb,&USR_Callbacks);	
	while(1)
		{
			USBH_Process(&USB_OTG_Core_dev, &USB_Host);
			if(bDeviceState==1)
			{ 
				if(USBH_Check_HIDCommDead(&USB_OTG_Core_dev,&HID_Machine))
				{ 	    
					USBH_HID_Reconnect();
				}				
				
			}else	
			{
				if(USBH_Check_EnumeDead(&USB_Host))	
				{ 	    
					USBH_HID_Reconnect();
				}			
			}

		}
	OSTaskDel(&USBTaskTCB,&err);
}
