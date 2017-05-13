#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define SYS_LIGHTWEIGHT_PROT    1

//NO_SYS==1:不使用操作系统
#define NO_SYS                  0  //使用UCOS操作系统

//使用4字节对齐模式
#define MEM_ALIGNMENT           4  

//MEM_SIZE:heap内存的大小,如果在应用中有大量数据发送的话这个值最好设置大一点 
#define MEM_SIZE                10*1500 //内存堆大小

//MEMP_NUM_PBUF:memp结构的pbuf数量,如果应用从ROM或者静态存储区发送大量数据时,这个值应该设置大一点
#define MEMP_NUM_PBUF           10

//MEMP_NUM_UDP_PCB:UDP协议控制块(PCB)数量.每个活动的UDP"连接"需要一个PCB.
#define MEMP_NUM_UDP_PCB        6

//MEMP_NUM_TCP_PCB:同时建立激活的TCP数量
#define MEMP_NUM_TCP_PCB        10

//MEMP_NUM_TCP_PCB_LISTEN:能够监听的TCP连接数量
#define MEMP_NUM_TCP_PCB_LISTEN 6

//MEMP_NUM_TCP_SEG:最多同时在队列中的TCP段数量
#define MEMP_NUM_TCP_SEG        20

//MEMP_NUM_SYS_TIMEOUT:能够同时激活的timeout个数
#define MEMP_NUM_SYS_TIMEOUT    5


/* ---------- Pbuf选项---------- */
//PBUF_POOL_SIZE:pbuf内存池个数. 
#define PBUF_POOL_SIZE          10

//PBUF_POOL_BUFSIZE:每个pbuf内存池大小. 
#define PBUF_POOL_BUFSIZE       1500


/* ---------- TCP选项---------- */
#define LWIP_TCP                1  //为1是使用TCP
#define TCP_TTL                 255//生存时间

/*当TCP的数据段超出队列时的控制位,当设备的内存过小的时候此项应为0*/

#undef TCP_QUEUE_OOSEQ
#define TCP_QUEUE_OOSEQ         		0							//μ±TCPμ?êy?Y??3?3??óáDê±μ???????,μ±éè±?μ??ú′?1yD?μ?ê±oò′???ó|?a0

#undef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE         		MAX_QUEUE_ENTRIES   		//tcpip′′?¨?÷??3ìê±μ????￠óê??′óD?

#undef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE       MAX_QUEUE_ENTRIES  

#undef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE         MAX_QUEUE_ENTRIES  
//最大TCP分段
#define TCP_MSS                 (1500 - 40)	  //TCP_MSS = (MTU - IP报头大小 - TCP报头大小

//TCP发送缓冲区大小(bytes).
#define TCP_SND_BUF             (4*TCP_MSS)

//TCP_SND_QUEUELEN: TCP发送缓冲区大小(pbuf).这个值最小为(2 * TCP_SND_BUF/TCP_MSS) 
#define TCP_SND_QUEUELEN        (4* TCP_SND_BUF/TCP_MSS)

//TCP发送窗口
#define TCP_WND                 (2*TCP_MSS)


/* ---------- ICMP选项---------- */
#define LWIP_ICMP                 1 //使用ICMP协议

/* ---------- DHCP选项---------- */
//当使用DHCP时此位应该为1,LwIP 0.5.1版本中没有DHCP服务.
#define LWIP_DHCP               1

/* ---------- UDP选项 ---------- */ 
#define LWIP_UDP                1 //使用UDP服务
#define UDP_TTL                 255 //UDP数据包生存时间


/* ---------- Statistics options ---------- */
#define LWIP_STATS 0
#define LWIP_PROVIDE_ERRNO 1


/*
   ----------------------------------------------
   ---------- SequentialAPI选项----------
   ----------------------------------------------
*/

//LWIP_NETCONN==1:使能NETCON函数(要求使用api_lib.c)

#define LWIP_NETCONN                    1 						
#define LWIP_SOCKET                     1							
#define LWIP_COMPAT_MUTEX               1		
#define LWIP_SO_RCVTIMEO                1 						


#define TCPIP_THREAD_PRIO				11		//仅次于lwip 接收中断							
#define TCPIP_THREAD_STACKSIZE          1024					
#define DEFAULT_UDP_RECVMBOX_SIZE       2048
#define DEFAULT_THREAD_STACKSIZE        512

//#define LWIP_DEBUG                     1 //开启DEBUG选项

#define ICMP_DEBUG                      LWIP_DBG_OFF //开启/关闭ICMPdebug

#endif /* __LWIPOPTS_H__ */


