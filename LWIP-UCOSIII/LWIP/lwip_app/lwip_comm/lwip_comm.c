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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ��Ӣ������
//lwipͨ������ ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/3/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
   
__lwip_dev lwipdev;						//lwip���ƽṹ�� 
struct netif lwip_netif;				//����һ��ȫ�ֵ�����ӿ�

extern u32 memp_get_memorysize(void);	//��memp.c���涨��
extern u8_t *memp_memory;				//��memp.c���涨��.
extern u8_t *ram_heap;					//��mem.c���涨��.

OS_SEM LwipDhcpSem;
void lwip_comm_dhcp_creat(void);
void lwip_comm_dhcp_delete(void);

void lwip_pkt_creat(void);
void lwip_pkt_delete(void);
//lwip��mem��memp���ڴ�����
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 lwip_comm_mem_malloc(void)
{
	u32 mempsize;
	u32 ramheapsize; 
	mempsize=memp_get_memorysize();			//�õ�memp_memory�����С

	memp_memory=mymalloc(mempsize);	//Ϊmemp_memory�����ڴ�
	ramheapsize=LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;//�õ�ram heap��С
	ram_heap=mymalloc(ramheapsize);	//Ϊram_heap�����ڴ� 
	if(!memp_memory||!ram_heap)//������ʧ�ܵ�
	{
		lwip_comm_mem_free();
		return 1;
	}

	return 0;	
}
//lwip��mem��memp�ڴ��ͷ�
void lwip_comm_mem_free(void)
{ 	
	myfree(memp_memory);
	myfree(ram_heap);
}
//lwip Ĭ��IP����
//lwipx:lwip���ƽṹ��ָ��
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	//Ĭ��Զ��IPΪ:192.168.1.100
	lwipx->remoteip[0]=192;	
	lwipx->remoteip[1]=168;
	lwipx->remoteip[2]=1;
	lwipx->remoteip[3]=100;
	//MAC��ַ����
	lwipx->mac[0]=enc28j60_dev.macaddr[0];
	lwipx->mac[1]=enc28j60_dev.macaddr[1];
	lwipx->mac[2]=enc28j60_dev.macaddr[2];
	lwipx->mac[3]=enc28j60_dev.macaddr[3];
	lwipx->mac[4]=enc28j60_dev.macaddr[4];
	lwipx->mac[5]=enc28j60_dev.macaddr[5]; 
	//Ĭ�ϱ���IPΪ:192.168.1.30
	lwipx->ip[0]=192;	
	lwipx->ip[1]=168;
	lwipx->ip[2]=1;
	lwipx->ip[3]=30;
	//Ĭ����������:255.255.255.0
	lwipx->netmask[0]=255;	
	lwipx->netmask[1]=255;
	lwipx->netmask[2]=255;
	lwipx->netmask[3]=0;
	//Ĭ������:192.168.1.1
	lwipx->gateway[0]=192;	
	lwipx->gateway[1]=168;
	lwipx->gateway[2]=1;
	lwipx->gateway[3]=1;	
	lwipx->dhcpstatus=0;//û��DHCP	
} 

//LWIP��ʼ��(LWIP������ʱ��ʹ��)
//����ֵ:0,�ɹ�
//      1,�ڴ����
//      2,DM9000��ʼ��ʧ��
//      3,�������ʧ��.
u8 lwip_comm_init(void)
{
	struct netif *Netif_Init_Flag;		//����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
	struct ip_addr ipaddr;  			//ip��ַ
	struct ip_addr netmask; 			//��������
	struct ip_addr gw;      			//Ĭ������ 
	if(lwip_comm_mem_malloc())return 1;	//�ڴ�����ʧ��
	if(ENC28J60_Init())return 2;		//��ʼ��ENC28J60
	tcpip_init(NULL,NULL);				//��ʼ��tcp ip�ں�,�ú�������ᴴ��tcpip_thread�ں�����
	lwip_comm_default_ip_set(&lwipdev);	//����Ĭ��IP����Ϣ
#if LWIP_DHCP		//ʹ�ö�̬IP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else				//ʹ�þ�̬IP
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
	printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
	printf("��̬IP��ַ........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	printf("��������..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
	printf("Ĭ������..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
#endif
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&tcpip_input);//�������б������һ������
	if(Netif_Init_Flag != NULL) 	//������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
	{
		netif_set_default(&lwip_netif); //����netifΪĬ������
		netif_set_up(&lwip_netif);		//��netif����
	}
	
	lwip_pkt_creat();	//����������������
	
#if	LWIP_DHCP
	OS_ERR err;
	CPU_TS ts;
	OSSemCreate(&LwipDhcpSem,0,0,&err);
	lwip_comm_dhcp_creat();		//����DHCP����
	OSSemPend(&LwipDhcpSem,0,OS_OPT_PEND_BLOCKING,&ts,&err);
	lwip_comm_dhcp_delete();//ɾ��DHCP����
#endif	
	return 0;//����OK.
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
		lwip_pkt_handle();//�����������ж�
	}
}
//�����յ����ݺ���� 
void lwip_pkt_handle(void)
{
	//�����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP���� 
	ethernetif_input(&lwip_netif);
}


//���ʹ����DHCP
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
//DHCP��������
void LwipDhcp_task(void *arg)
{
	u32 ip=0,netmask=0,gw=0;
	dhcp_start(&lwip_netif);//����DHCP 
	lwipdev.dhcpstatus=0;	//����DHCP
	printf("���ڲ���DHCP������,���Ե�...........\r\n");   
	while(1)
	{ 
		printf("���ڻ�ȡ��ַ...\r\n");
		ip=lwip_netif.ip_addr.addr;		//��ȡ��IP��ַ
		netmask=lwip_netif.netmask.addr;//��ȡ��������
		gw=lwip_netif.gw.addr;			//��ȡĬ������ 
		if(ip!=0)   					//����ȷ��ȡ��IP��ַ��ʱ��
		{
			lwipdev.dhcpstatus=2;	//DHCP�ɹ�
 			printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
			//������ͨ��DHCP��ȡ����IP��ַ
			lwipdev.ip[3]=(uint8_t)(ip>>24); 
			lwipdev.ip[2]=(uint8_t)(ip>>16);
			lwipdev.ip[1]=(uint8_t)(ip>>8);
			lwipdev.ip[0]=(uint8_t)(ip);
			printf("ͨ��DHCP��ȡ��IP��ַ..............%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			//����ͨ��DHCP��ȡ�������������ַ
			lwipdev.netmask[3]=(uint8_t)(netmask>>24);
			lwipdev.netmask[2]=(uint8_t)(netmask>>16);
			lwipdev.netmask[1]=(uint8_t)(netmask>>8);
			lwipdev.netmask[0]=(uint8_t)(netmask);
			printf("ͨ��DHCP��ȡ����������............%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			//������ͨ��DHCP��ȡ����Ĭ������
			lwipdev.gateway[3]=(uint8_t)(gw>>24);
			lwipdev.gateway[2]=(uint8_t)(gw>>16);
			lwipdev.gateway[1]=(uint8_t)(gw>>8);
			lwipdev.gateway[0]=(uint8_t)(gw);
			printf("ͨ��DHCP��ȡ����Ĭ������..........%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			break;
		}else if(lwip_netif.dhcp->tries>LWIP_MAX_DHCP_TRIES) //ͨ��DHCP�����ȡIP��ַʧ��,�ҳ�������Դ���
		{  
			lwipdev.dhcpstatus=0XFF;//DHCPʧ��.
			//ʹ�þ�̬IP��ַ
			IP4_ADDR(&(lwip_netif.ip_addr),lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			IP4_ADDR(&(lwip_netif.netmask),lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			IP4_ADDR(&(lwip_netif.gw),lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			printf("DHCP����ʱ,ʹ�þ�̬IP��ַ!\r\n");
			printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
			printf("��̬IP��ַ........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
			printf("��������..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
			printf("Ĭ������..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			break;
		}  
		delay_ms(250); //��ʱ250ms
	}
	OS_ERR err;
	OS_SemPost(&LwipDhcpSem,
			   OS_OPT_POST_1,	//һ��
			   0,
			   &err);
}
#endif 



























