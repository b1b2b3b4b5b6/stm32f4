#include "udp_demo.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ENC28J60ģ��
//NETCONN API��̷�ʽ��UDP���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define LWIPUDP_TASK_PRIO		16
#define LWIPUDP_STK_SIZE 		256
OS_TCB LwipUdpTaskTCB;
CPU_STK LWIPUDP_TASK_STK[LWIPUDP_STK_SIZE];
void LwipUdp_task(void *p_arg);


u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ�����
//UDP������������
const u8 *udp_demo_sendbuf="ENC28J60 NETCONN UDP demo send data\r\n";
u8 udp_flag;							//UDP���ݷ��ͱ�־λ

//udp������
void LwipUdp_task(void *p_arg)
{
	err_t err;
	CPU_SR_ALLOC();
	
	static struct netconn *udpconn;
	static struct netbuf  *recvbuf;
	static struct netbuf  *sentbuf;
	struct ip_addr destipaddr;
	u32 data_len = 0;
	struct pbuf *q;
	
	LWIP_UNUSED_ARG(p_arg);
	udpconn = netconn_new(NETCONN_UDP);  //����һ��UDP����
	udpconn->recv_timeout = 10;  		
	
	if(udpconn != NULL)  //����UDP���ӳɹ�
	{
		printf("UDP���Ӵ����ɹ�\r\n");
		err = netconn_bind(udpconn,IP_ADDR_ANY,UDP_DEMO_PORT); 			//���ñ��ض˿�
		IP4_ADDR(&destipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]); //����Ŀ��IP��ַ
		netconn_connect(udpconn,&destipaddr,UDP_DEMO_PORT); 	//���ӵ�Զ������
		if(err == ERR_OK)//�����
		{
			printf("UDP�󶨳ɹ�\r\n");
			while(1)
			{
				if((udp_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					sentbuf = netbuf_new();
					netbuf_alloc(sentbuf,strlen((char *)udp_demo_sendbuf));
					sentbuf->p->payload = (char*)udp_demo_sendbuf;   	//ָudp_demo_sendbuf��
					err = netconn_send(udpconn,sentbuf);  	//��netbuf�е����ݷ��ͳ�ȥ
					if(err != ERR_OK)
					{
						printf("UDP����ʧ��\r\n");
						netbuf_delete(sentbuf);      //ɾ��buf
					}
					else printf("UDP���ͳɹ�\r\n");
					udp_flag &= ~LWIP_SEND_DATA;	//������ݷ��ͱ�־
					netbuf_delete(sentbuf);      	//ɾ��buf
				}	
				
				netconn_recv(udpconn,&recvbuf); //��������
				if(recvbuf != NULL)          //���յ�����
				{ 
					OS_CRITICAL_ENTER(); //���ж�
					memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
						else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����UDP��������,����	
					}
					OS_CRITICAL_EXIT();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣
					printf("%s\r\n",udp_demo_recvbuf);  //��ӡ���յ�������
					netbuf_delete(recvbuf);      //ɾ��buf
				}else delay_ms(5);  //��ʱ5ms
			}
		}else printf("UDP��ʧ��\r\n");
	}else printf("UDP���Ӵ���ʧ��\r\n");
}


//����UDP�߳�
//����ֵ:0 UDP�����ɹ�
//		���� UDP����ʧ��
void udp_demo_init(void)
{
	CPU_SR_ALLOC();
	OS_ERR err;
	OS_CRITICAL_ENTER(); 
	
	OSTaskCreate((OS_TCB 	* )&LwipUdpTaskTCB,
				 (CPU_CHAR	* )"LwipUdp task",
				 (OS_TASK_PTR )LwipUdp_task,
				 (void		* )0,
				 (OS_PRIO	  )LWIPUDP_TASK_PRIO,
				 (CPU_STK   * )&LWIPUDP_TASK_STK[0],
				 (CPU_STK_SIZE)LWIPUDP_STK_SIZE/10,
				 (CPU_STK_SIZE)LWIPUDP_STK_SIZE,
				 (OS_MSG_QTY  )0,
				 (OS_TICK	  )0,
				 (void   	* )0,
				 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
				 (OS_ERR 	* )&err); 
	
	OS_CRITICAL_EXIT(); 
}

