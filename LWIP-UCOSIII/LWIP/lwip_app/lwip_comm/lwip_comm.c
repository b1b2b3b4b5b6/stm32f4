#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "ethernetif.h" 
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h" 
#include "malloc.h"
#include "delay.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 精英开发板
//lwip通用驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/3/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
   
__lwip_dev lwipdev;						//lwip控制结构体 
struct netif lwip_netif;				//定义一个全局的网络接口

extern u32 memp_get_memorysize(void);	//在memp.c里面定义
extern u8_t *memp_memory;				//在memp.c里面定义.
extern u8_t *ram_heap;					//在mem.c里面定义.

OS_SEM LwipDhcpSem;
void lwip_comm_dhcp_creat(void);
void lwip_comm_dhcp_delete(void);

void lwip_pkt_creat(void);
void lwip_pkt_delete(void);
//lwip中mem和memp的内存申请
//返回值:0,成功;
//    其他,失败
u8 lwip_comm_mem_malloc(void)
{
	u32 mempsize;
	u32 ramheapsize; 
	mempsize=memp_get_memorysize();			//得到memp_memory数组大小

	memp_memory=mymalloc(mempsize);	//为memp_memory申请内存
	ramheapsize=LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;//得到ram heap大小
	ram_heap=mymalloc(ramheapsize);	//为ram_heap申请内存 
	if(!memp_memory||!ram_heap)//有申请失败的
	{
		lwip_comm_mem_free();
		return 1;
	}

	return 0;	
}
//lwip中mem和memp内存释放
void lwip_comm_mem_free(void)
{ 	
	myfree(memp_memory);
	myfree(ram_heap);
}
//lwip 默认IP设置
//lwipx:lwip控制结构体指针
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	//默认远端IP为:192.168.1.100
	lwipx->remoteip[0]=192;	
	lwipx->remoteip[1]=168;
	lwipx->remoteip[2]=1;
	lwipx->remoteip[3]=100;
	//MAC地址设置
	lwipx->mac[0]=enc28j60_dev.macaddr[0];
	lwipx->mac[1]=enc28j60_dev.macaddr[1];
	lwipx->mac[2]=enc28j60_dev.macaddr[2];
	lwipx->mac[3]=enc28j60_dev.macaddr[3];
	lwipx->mac[4]=enc28j60_dev.macaddr[4];
	lwipx->mac[5]=enc28j60_dev.macaddr[5]; 
	//默认本地IP为:192.168.1.30
	lwipx->ip[0]=192;	
	lwipx->ip[1]=168;
	lwipx->ip[2]=1;
	lwipx->ip[3]=30;
	//默认子网掩码:255.255.255.0
	lwipx->netmask[0]=255;	
	lwipx->netmask[1]=255;
	lwipx->netmask[2]=255;
	lwipx->netmask[3]=0;
	//默认网关:192.168.1.1
	lwipx->gateway[0]=192;	
	lwipx->gateway[1]=168;
	lwipx->gateway[2]=1;
	lwipx->gateway[3]=1;	
	lwipx->dhcpstatus=0;//没有DHCP	
} 

//LWIP初始化(LWIP启动的时候使用)
//返回值:0,成功
//      1,内存错误
//      2,DM9000初始化失败
//      3,网卡添加失败.
u8 lwip_comm_init(void)
{
	struct netif *Netif_Init_Flag;		//调用netif_add()函数时的返回值,用于判断网络初始化是否成功
	struct ip_addr ipaddr;  			//ip地址
	struct ip_addr netmask; 			//子网掩码
	struct ip_addr gw;      			//默认网关 
	if(lwip_comm_mem_malloc())return 1;	//内存申请失败
	if(ENC28J60_Init())return 2;		//初始化ENC28J60
	tcpip_init(NULL,NULL);				//初始化tcp ip内核,该函数里面会创建tcpip_thread内核任务
	lwip_comm_default_ip_set(&lwipdev);	//设置默认IP等信息
#if LWIP_DHCP		//使用动态IP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else				//使用静态IP
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
	printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
	printf("静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	printf("子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
	printf("默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
#endif
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&tcpip_input);//向网卡列表中添加一个网口
	if(Netif_Init_Flag != NULL) 	//网口添加成功后,设置netif为默认值,并且打开netif网口
	{
		netif_set_default(&lwip_netif); //设置netif为默认网口
		netif_set_up(&lwip_netif);		//打开netif网口
	}
	
	lwip_pkt_creat();	//创建接收数据任务
	
#if	LWIP_DHCP
	OS_ERR err;
	CPU_TS ts;
	OSSemCreate(&LwipDhcpSem,0,0,&err);
	lwip_comm_dhcp_creat();		//创建DHCP任务
	OSSemPend(&LwipDhcpSem,0,OS_OPT_PEND_BLOCKING,&ts,&err);
	lwip_comm_dhcp_delete();//删除DHCP任务
#endif	
	return 0;//操作OK.
}   

#define LWIPPKT_TASK_PRIO		10
#define LWIPPKT_STK_SIZE 		200
OS_TCB LwipPktTaskTCB;
CPU_STK LWIPPKT_TASK_STK[LWIPPKT_STK_SIZE];
void LwipPkt_task(void *p_arg);

void lwip_pkt_creat(void)
{
	CPU_SR_ALLOC();
	OS_ERR err;
	OS_CRITICAL_ENTER(); 
	
	OSTaskCreate((OS_TCB 	* )&LwipPktTaskTCB,
				 (CPU_CHAR	* )"LwipPkt task",
				 (OS_TASK_PTR )LwipPkt_task,
				 (void		* )0,
				 (OS_PRIO	  )LWIPPKT_TASK_PRIO,
				 (CPU_STK   * )&LWIPPKT_TASK_STK[0],
				 (CPU_STK_SIZE)LWIPPKT_STK_SIZE/10,
				 (CPU_STK_SIZE)LWIPPKT_STK_SIZE,
				 (OS_MSG_QTY  )0,
				 (OS_TICK	  )0,
				 (void   	* )0,
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR 	* )&err); 
	
	OS_CRITICAL_EXIT(); 
}

void lwip_pkt_delete(void)
{
	OS_ERR err;
 		
	OSTaskDel(&LwipPktTaskTCB,&err);
}

void LwipPkt_task(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	while(1)
	{
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);
		lwip_pkt_handle();//接收数据清中断
	}
}
//当接收到数据后调用 
void lwip_pkt_handle(void)
{
	//从网络缓冲区中读取接收到的数据包并将其发送给LWIP处理 
	ethernetif_input(&lwip_netif);
}


//如果使能了DHCP
#if LWIP_DHCP

#define LWIPDHCP_TASK_PRIO		11
#define LWIPDHCP_STK_SIZE 		200
OS_TCB LwipDhcpTaskTCB;
CPU_STK LWIPDHCP_TASK_STK[LWIPDHCP_STK_SIZE];
void LwipDhcp_task(void *p_arg);

void lwip_comm_dhcp_creat(void)
{
	CPU_SR_ALLOC();
	OS_ERR err;
	OS_CRITICAL_ENTER(); 
	
	OSTaskCreate((OS_TCB 	* )&LwipDhcpTaskTCB,
				 (CPU_CHAR	* )"LwipDhcp task",
				 (OS_TASK_PTR )LwipDhcp_task,
				 (void		* )0,
				 (OS_PRIO	  )LWIPDHCP_TASK_PRIO,
				 (CPU_STK   * )&LWIPDHCP_TASK_STK[0],
				 (CPU_STK_SIZE)LWIPDHCP_STK_SIZE/10,
				 (CPU_STK_SIZE)LWIPDHCP_STK_SIZE,
				 (OS_MSG_QTY  )0,
				 (OS_TICK	  )0,
				 (void   	* )0,
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR 	* )&err); 
	
	OS_CRITICAL_EXIT(); 
}

void lwip_comm_dhcp_delete(void)
{
	OS_ERR err;
	dhcp_stop(&lwip_netif); 		
	OSTaskDel(&LwipDhcpTaskTCB,&err);
}
//DHCP处理任务
void LwipDhcp_task(void *arg)
{
	u32 ip=0,netmask=0,gw=0;
	dhcp_start(&lwip_netif);//开启DHCP 
	lwipdev.dhcpstatus=0;	//正在DHCP
	printf("正在查找DHCP服务器,请稍等...........\r\n");   
	while(1)
	{ 
		printf("正在获取地址...\r\n");
		ip=lwip_netif.ip_addr.addr;		//读取新IP地址
		netmask=lwip_netif.netmask.addr;//读取子网掩码
		gw=lwip_netif.gw.addr;			//读取默认网关 
		if(ip!=0)   					//当正确读取到IP地址的时候
		{
			lwipdev.dhcpstatus=2;	//DHCP成功
 			printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
			//解析出通过DHCP获取到的IP地址
			lwipdev.ip[3]=(uint8_t)(ip>>24); 
			lwipdev.ip[2]=(uint8_t)(ip>>16);
			lwipdev.ip[1]=(uint8_t)(ip>>8);
			lwipdev.ip[0]=(uint8_t)(ip);
			printf("通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			//解析通过DHCP获取到的子网掩码地址
			lwipdev.netmask[3]=(uint8_t)(netmask>>24);
			lwipdev.netmask[2]=(uint8_t)(netmask>>16);
			lwipdev.netmask[1]=(uint8_t)(netmask>>8);
			lwipdev.netmask[0]=(uint8_t)(netmask);
			printf("通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			//解析出通过DHCP获取到的默认网关
			lwipdev.gateway[3]=(uint8_t)(gw>>24);
			lwipdev.gateway[2]=(uint8_t)(gw>>16);
			lwipdev.gateway[1]=(uint8_t)(gw>>8);
			lwipdev.gateway[0]=(uint8_t)(gw);
			printf("通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			break;
		}else if(lwip_netif.dhcp->tries>LWIP_MAX_DHCP_TRIES) //通过DHCP服务获取IP地址失败,且超过最大尝试次数
		{  
			lwipdev.dhcpstatus=0XFF;//DHCP失败.
			//使用静态IP地址
			IP4_ADDR(&(lwip_netif.ip_addr),lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			IP4_ADDR(&(lwip_netif.netmask),lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			IP4_ADDR(&(lwip_netif.gw),lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			printf("DHCP服务超时,使用静态IP地址!\r\n");
			printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
			printf("静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			printf("子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			printf("默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			break;
		}  
		delay_ms(250); //延时250ms
	}
	OS_ERR err;
	OS_SemPost(&LwipDhcpSem,
			   OS_OPT_POST_1,	//一次
			   0,
			   &err);
}
#endif 



























