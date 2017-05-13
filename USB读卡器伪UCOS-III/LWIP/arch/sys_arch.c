
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


//����Ϣָ��Ϊ��ʱ,ָ��һ������pvNullPointer��ָ���ֵ.
//��UCOS�����OSQPost()�е�msg==NULL�᷵��һ��OS_ERR_POST_NULL
//����,����lwip�л����sys_mbox_post(mbox,NULL)����һ������Ϣ,����
//�ڱ������а�NULL���һ������ָ��0Xffffffff
const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;
 

//����һ����Ϣ����
//*mbox:��Ϣ����
//size:�����С
//����ֵ:ERR_OK,�����ɹ�
//         ����,����ʧ��
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
//�ͷŲ�ɾ��һ����Ϣ����
//*mbox:Ҫɾ������Ϣ����
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
//����Ϣ�����з���һ����Ϣ(���뷢�ͳɹ�)
//*mbox:��Ϣ����
//*msg:Ҫ���͵���Ϣ
void sys_mbox_post(sys_mbox_t *mbox,void *msg)
{    
	OS_ERR err=OS_ERR_Q_MAX;//�������һ�����󷵻�ֵ
	while(err!=OS_ERR_NONE)
	{
		OSQPost(*mbox,
				msg,
				NULL,
				OS_OPT_POST_FIFO,
				&err);
	}
	
}
//������һ����Ϣ���䷢����Ϣ
//�˺��������sys_mbox_post����ֻ����һ����Ϣ��
//����ʧ�ܺ󲻻᳢�Եڶ��η���
//*mbox:��Ϣ����
//*msg:Ҫ���͵���Ϣ
//����ֵ:ERR_OK,����OK
// 	     ERR_MEM,����ʧ��
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

//�ȴ������е���Ϣ
//*mbox:��Ϣ����
//*msg:��Ϣ
//timeout:��ʱʱ�䣬���timeoutΪ0�Ļ�,��һֱ�ȴ�
//����ֵ:��timeout��Ϊ0ʱ����ɹ��Ļ��ͷ��صȴ���ʱ�䣬
//		ʧ�ܵĻ��ͷ��س�ʱSYS_ARCH_TIMEOUT
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
//���Ի�ȡ��Ϣ
//*mbox:��Ϣ����
//*msg:��Ϣ
//����ֵ:�ȴ���Ϣ���õ�ʱ��/SYS_ARCH_TIMEOUT
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
//���һ����Ϣ�����Ƿ���Ч
//*mbox:��Ϣ����
//����ֵ:1,��Ч.
//      0,��Ч
int sys_mbox_valid(sys_mbox_t *mbox)
{  
	if(mbox==0) return 0;
	else return 1;
} 
//����һ����Ϣ����Ϊ��Ч
//*mbox:��Ϣ����
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
//����һ���ź���
//*sem:�������ź���
//count:�ź���ֵ
//����ֵ:ERR_OK,����OK
// 	     ERR_MEM,����ʧ��
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
//�ȴ�һ���ź���
//*sem:Ҫ�ȴ����ź���
//timeout:��ʱʱ��
//����ֵ:��timeout��Ϊ0ʱ����ɹ��Ļ��ͷ��صȴ���ʱ�䣬
//		ʧ�ܵĻ��ͷ��س�ʱSYS_ARCH_TIMEOUT
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
//����һ���ź���
//sem:�ź���ָ��
void sys_sem_signal(sys_sem_t *sem)
{
	OS_ERR err;
	OS_SemPost(*sem,
			   OS_OPT_POST_1,	//һ��
			   0,
			   &err);
}
//�ͷŲ�ɾ��һ���ź���
//sem:�ź���ָ��
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
//��ѯһ���ź�����״̬,��Ч����Ч
//sem:�ź���ָ��
//����ֵ:1,��Ч.
//      0,��Ч
int sys_sem_valid(sys_sem_t *sem)
{
	if(*sem==0) return 0;
	else return 1;
} 
//����һ���ź�����Ч
//sem:�ź���ָ��
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
//arch��ʼ��
void sys_init(void)
{ 
    //����,�����ڸú���,�����κ�����
} 

//����һ���½���
//*name:��������
//thred:����������
//*arg:�����������Ĳ���
//stacksize:��������Ķ�ջ��С
//prio:������������ȼ�
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	void *p_ThreadTaskTCB;
	void *p_THREAD_TASK_STK;
	OS_CRITICAL_ENTER();	//�����ٽ���
	
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
	
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	return (u8)prio;
} 
//lwip��ʱ����
//ms:Ҫ��ʱ��ms��
void sys_msleep(u32_t ms)
{
	delay_ms(ms);
}
//��ȡϵͳʱ��,LWIP1.4.1���ӵĺ���
//����ֵ:��ǰϵͳʱ��(��λ:����)
u32_t sys_now(void)
{
	OS_ERR err;
	u32_t ucos_time, lwip_time;
	ucos_time=OSTimeGet(&err);	
	lwip_time=(ucos_time/OSCfg_TickRate_Hz*1000);//lwip�Ժ����ʱ
	return lwip_time;
}
