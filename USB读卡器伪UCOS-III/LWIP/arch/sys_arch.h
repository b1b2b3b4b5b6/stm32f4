
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
 
#define MAX_QUEUES        		10	// 消息邮箱的数量
#define MAX_QUEUE_ENTRIES 		20	// 每个消息邮箱的大小

typedef OS_SEM *sys_sem_t;    //LWIP使用的信号量
typedef OS_MUTEX *sys_mutex_t; 	//LWIP使用的互斥信号量
typedef OS_Q *sys_mbox_t;    //LWIP使用的消息邮箱,其实就是UCOS中的消息队列
typedef u8 sys_thread_t;     //线程ID,也就是任务优先级



#endif 



































