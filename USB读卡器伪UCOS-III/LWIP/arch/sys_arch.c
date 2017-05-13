
#define SYS_ARCH_GLOBALS

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/lwip_sys.h"
#include "lwip/mem.h"
#include "includes.h"
#include "delay.h"
#include "arch/sys_arch.h"
#include "malloc.h"
#include "includes.h"


//当消息指针为空时,指向一个常量pvNullPointer所指向的值.
//在UCOS中如果OSQPost()中的msg==NULL会返回一条OS_ERR_POST_NULL
//错误,而在lwip中会调用sys_mbox_post(mbox,NULL)发送一条空消息,我们
//在本函数中把NULL变成一个常量指针0Xffffffff
const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;
 

//创建一个消息邮箱
//*mbox:消息邮箱
//size:邮箱大小
//返回值:ERR_OK,创建成功
//         其他,创建失败
err_t sys_mbox_new( sys_mbox_t *mbox, int size)
{
	OS_ERR err;
	
	INTX_DISABLE();
	*mbox=mymalloc(sizeof(OS_Q));
	if(*mbox==NULL) 
	{
		myfree(*mbox);
		printf("sys_mbox_new(malloc fail!)");
	}
	INTX_ENABLE();
	if(size>MAX_QUEUE_ENTRIES)size=MAX_QUEUE_ENTRIES;		
	OSQCreate(	*mbox,
				NULL,
				size,
				&err);
	
	if(err==OS_ERR_NONE) return ERR_OK;
	else 
	{
		INTX_DISABLE();
		myfree(*mbox);
		INTX_ENABLE();
		return ERR_MEM;
	}
} 
//释放并删除一个消息邮箱
//*mbox:要删除的消息邮箱
void sys_mbox_free(sys_mbox_t * mbox)
{
	OS_ERR err;
	OSQDel(*mbox,
			OS_OPT_DEL_ALWAYS,
			&err);
	INTX_DISABLE();
	myfree(*mbox);
	INTX_ENABLE();	
}
//向消息邮箱中发送一条消息(必须发送成功)
//*mbox:消息邮箱
//*msg:要发送的消息
void sys_mbox_post(sys_mbox_t *mbox,void *msg)
{    
	OS_ERR err=OS_ERR_Q_MAX;//随便设置一个错误返回值
	while(err!=OS_ERR_NONE)
	{
		OSQPost(*mbox,
				msg,
				NULL,
				OS_OPT_POST_FIFO,
				&err);
	}
	
}
//尝试向一个消息邮箱发送消息
//此函数相对于sys_mbox_post函数只发送一次消息，
//发送失败后不会尝试第二次发送
//*mbox:消息邮箱
//*msg:要发送的消息
//返回值:ERR_OK,发送OK
// 	     ERR_MEM,发送失败
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{ 
	OS_ERR err;
	OSQPost(*mbox,
			msg,
			NULL,
			OS_OPT_POST_FIFO,
			&err);	
	if(err==OS_ERR_NONE)	return ERR_OK;
	else	return ERR_MEM;
}

//等待邮箱中的消息
//*mbox:消息邮箱
//*msg:消息
//timeout:超时时间，如果timeout为0的话,就一直等待
//返回值:当timeout不为0时如果成功的话就返回等待的时间，
//		失败的话就返回超时SYS_ARCH_TIMEOUT
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{ 
	CPU_TS ts_old,ts_now;
	CPU_TS ucos_timeout;
	OS_ERR err;
	OS_MSG_SIZE size;
	if(timeout!=0)
	{
		ucos_timeout=timeout*1000/OSCfg_TickRate_Hz;
		if(ucos_timeout<1)ucos_timeout=1;
	}else ucos_timeout = 0; 

	ts_old=OSTimeGet(&err);
	
	*msg=OSQPend(*mbox,
			ucos_timeout,
			OS_OPT_PEND_BLOCKING,
			&size,
			&ts_now,
			&err);
	if(err==OS_ERR_NONE)	return (ts_now-ts_old)/OSCfg_TickRate_Hz*1000;
	else return SYS_ARCH_TIMEOUT;	
}
//尝试获取消息
//*mbox:消息邮箱
//*msg:消息
//返回值:等待消息所用的时间/SYS_ARCH_TIMEOUT
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	CPU_TS ts_now;
	OS_ERR err;
	OS_MSG_SIZE size;
	
	*msg=OSQPend(*mbox,
				  0,
				  OS_OPT_PEND_NON_BLOCKING,
				  &size,
				  &ts_now,
				  &err);
	if(err==OS_ERR_NONE)	return 0;
	else return SYS_MBOX_EMPTY;	
}
//检查一个消息邮箱是否有效
//*mbox:消息邮箱
//返回值:1,有效.
//      0,无效
int sys_mbox_valid(sys_mbox_t *mbox)
{  
	if(mbox==0) return 0;
	else return 1;
} 
//设置一个消息邮箱为无效
//*mbox:消息邮箱
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	OS_ERR err;
	OSQDel(*mbox,
			  OS_OPT_DEL_ALWAYS,
			  &err);
	
	INTX_DISABLE();
	myfree(*mbox);
	INTX_ENABLE();
} 
//创建一个信号量
//*sem:创建的信号量
//count:信号量值
//返回值:ERR_OK,创建OK
// 	     ERR_MEM,创建失败
err_t sys_sem_new(sys_sem_t * sem, u8_t count)
{  
	OS_ERR err;
	
	INTX_DISABLE();
	*sem=mymalloc(sizeof(OS_SEM));
	if(*sem==NULL) 
	{
		myfree(*sem);
		printf("sys_sem_new(malloc fail!)");
	}
	INTX_ENABLE();
	OSSemCreate(*sem,0,count,&err);
	
	if(err==OS_ERR_NONE) return ERR_OK;
	else 
	{
		INTX_DISABLE();
		myfree(*sem);
		INTX_ENABLE();
		return ERR_MEM;
	}
} 
//等待一个信号量
//*sem:要等待的信号量
//timeout:超时时间
//返回值:当timeout不为0时如果成功的话就返回等待的时间，
//		失败的话就返回超时SYS_ARCH_TIMEOUT
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{ 
	CPU_TS ts_old,ts_now;
	CPU_TS ucos_timeout;
	OS_ERR err;
	
	if(timeout!=0)
	{
		ucos_timeout=timeout*1000/OSCfg_TickRate_Hz;
		if(ucos_timeout<1)ucos_timeout=1;
	}else ucos_timeout = 0; 
	
	ts_old=OSTimeGet(&err);
	OSSemPend(*sem,
			  ucos_timeout,
			  OS_OPT_PEND_BLOCKING,
			  &ts_now,
			  &err);
	if(err==OS_ERR_NONE)	return (ts_now-ts_old)/OSCfg_TickRate_Hz*1000;
	else return SYS_ARCH_TIMEOUT;
}
//发送一个信号量
//sem:信号量指针
void sys_sem_signal(sys_sem_t *sem)
{
	OS_ERR err;
	OS_SemPost(*sem,
			   OS_OPT_POST_1,	//一次
			   0,
			   &err);
}
//释放并删除一个信号量
//sem:信号量指针
void sys_sem_free(sys_sem_t *sem)
{
	OS_ERR err;
	OSSemDel(*sem,
			  OS_OPT_DEL_ALWAYS,
			  &err);
	INTX_DISABLE();
	myfree(*sem);
	INTX_ENABLE();
} 
//查询一个信号量的状态,无效或有效
//sem:信号量指针
//返回值:1,有效.
//      0,无效
int sys_sem_valid(sys_sem_t *sem)
{
	if(*sem==0) return 0;
	else return 1;
} 
//设置一个信号量无效
//sem:信号量指针
void sys_sem_set_invalid(sys_sem_t *sem)
{
	OS_ERR err;
	OSSemDel(*sem,
			  OS_OPT_DEL_ALWAYS,
			  &err);
	
	INTX_DISABLE();
	myfree(*sem);
	INTX_ENABLE();
	
	*sem=NULL;
} 
//arch初始化
void sys_init(void)
{ 
    //这里,我们在该函数,不做任何事情
} 

//创建一个新进程
//*name:进程名称
//thred:进程任务函数
//*arg:进程任务函数的参数
//stacksize:进程任务的堆栈大小
//prio:进程任务的优先级
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	void *p_ThreadTaskTCB;
	void *p_THREAD_TASK_STK;
	OS_CRITICAL_ENTER();	//进入临界区
	
	p_ThreadTaskTCB=mymalloc(sizeof(OS_TCB));
	p_THREAD_TASK_STK=mymalloc(stacksize*4);
	
	if((p_ThreadTaskTCB==NULL)||(p_THREAD_TASK_STK==NULL))
	{	
		myfree(p_ThreadTaskTCB);
		myfree(p_THREAD_TASK_STK);
		printf("sys_thread_new(malloc fail!)");
	}
	
	OSTaskCreate((OS_TCB*     )p_ThreadTaskTCB,
				 (CPU_CHAR*   )name,
				 (OS_TASK_PTR )thread,
				 (void*       )arg,
				 (OS_PRIO	  )prio,
				 (CPU_STK*    )p_THREAD_TASK_STK,
				 (CPU_STK_SIZE)stacksize/10,
				 (CPU_STK_SIZE)stacksize,
				 (OS_MSG_QTY  )0,
				 (OS_TICK	  )0,
				 (void*       )0,
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR*     )&err);
	
	OS_CRITICAL_EXIT();	//退出临界区
	return (u8)prio;
} 
//lwip延时函数
//ms:要延时的ms数
void sys_msleep(u32_t ms)
{
	delay_ms(ms);
}
//获取系统时间,LWIP1.4.1增加的函数
//返回值:当前系统时间(单位:毫秒)
u32_t sys_now(void)
{
	OS_ERR err;
	u32_t ucos_time, lwip_time;
	ucos_time=OSTimeGet(&err);	
	lwip_time=(ucos_time/OSCfg_TickRate_Hz*1000);//lwip以毫秒计时
	return lwip_time;
}
