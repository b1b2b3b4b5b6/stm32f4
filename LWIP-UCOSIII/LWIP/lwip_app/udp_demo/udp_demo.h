#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include "includes.h"
#include "lwip_comm.h"
#include "lwip/api.h"
#include "lwip/lwip_sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ս�������� V3
//NETCONN API��̷�ʽ��UDP���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/3/21
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
 
#define UDP_DEMO_RX_BUFSIZE		2000		//����udp���������ݳ���
#define UDP_DEMO_PORT			8089	//����udp���ӵı��غ�Զ�̶˿ں�
#define LWIP_SEND_DATA			0X80    //���������ݷ���

extern u8 udp_flag;		//UDP���ݷ��ͱ�־λ
void udp_demo_init(void);
#endif

