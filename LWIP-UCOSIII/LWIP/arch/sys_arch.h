
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__ 
#include <includes.h>
#include "arch/cc.h"
#include "includes.h"
 
#ifdef SYS_ARCH_GLOBALS
#define SYS_ARCH_EXT
#else
#define SYS_ARCH_EXT extern
#endif
 
#define MAX_QUEUES        		10	// ��Ϣ���������
#define MAX_QUEUE_ENTRIES 		20	// ÿ����Ϣ����Ĵ�С

typedef OS_SEM *sys_sem_t;    //LWIPʹ�õ��ź���
typedef OS_MUTEX *sys_mutex_t; 	//LWIPʹ�õĻ����ź���
typedef OS_Q *sys_mbox_t;    //LWIPʹ�õ���Ϣ����,��ʵ����UCOS�е���Ϣ����
typedef u8 sys_thread_t;     //�߳�ID,Ҳ�����������ȼ�



#endif 



































