#include "usbh_usr.h"
#include "usb_hcd_int.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"   
#include "init.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//USBH-USR 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/23
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
//表示USB连接状态
//0,没有连接;
//1,已经连接;
vu8 bDeviceState=0;		//默认没有连接  

OS_Q HID_Q;

extern USBH_HOST  USB_Host;
extern USB_OTG_CORE_HANDLE USB_OTG_Core_dev;
u8 USB_FIRST_PLUGIN_FLAG=0;	//USB第一次插入标志,如果为1,说明是第一次插入

//USB OTG 中断服务函数
//处理所有USB中断
void OTG_FS_IRQHandler(void)
{ 
	USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
}  

//USB HOST 用户回调函数.
USBH_Usr_cb_TypeDef USR_Callbacks =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  NULL,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
};
 
//USB HOST 初始化 
void USBH_USR_Init(void)
{
	OS_ERR err;
	OSQCreate(&HID_Q,NULL,100,&err);
	//printf("USB OTG FS MSC Host\r\n");
	//printf("> USB Host library started.\r\n");
	//printf("  USB Host Library v2.1.0\r\n\r\n");

	
}
//检测到U盘插入
void USBH_USR_DeviceAttached(void)//U盘插入
{ 
	//printf("检测到USB设备插入!\r\n");
}
//检测到U盘拔出
void USBH_USR_DeviceDisconnected (void)//U盘移除
{ 
	//printf("USB设备拔出!\r\n");
	bDeviceState=0;	//USB设备拔出了
}  
//复位从机
void USBH_USR_ResetDevice(void)
{
	//printf("复位设备...\r\n");
}
//检测到从机速度
//DeviceSpeed:从机速度(0,1,2 / 其他)
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if(DeviceSpeed==HPRT0_PRTSPD_HIGH_SPEED)
	{
	//	printf("高速(HS)USB设备!\r\n");
 	}  
	else if(DeviceSpeed==HPRT0_PRTSPD_FULL_SPEED)
	{
	//	printf("全速(FS)USB设备!\r\n"); 
	}
	else if(DeviceSpeed==HPRT0_PRTSPD_LOW_SPEED)
	{
		//printf("低速(LS)USB设备!\r\n");  
	}
	else
	{
		//printf("设备错误!\r\n");  
	}
}
//检测到从机的描述符
//DeviceDesc:设备描述符指针
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
	USBH_DevDesc_TypeDef *hs;
	hs=DeviceDesc;   
	//printf("VID: %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
	//printf("PID: %04Xh\r\n" , (uint32_t)(*hs).idProduct); 
}
//从机地址分配成功
void USBH_USR_DeviceAddressAssigned(void)
{
	//printf("从机地址分配成功!\r\n");   
}
//配置描述符获有效
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id; 
	id = itfDesc;   
	if((*id).bInterfaceClass==0x08)
	{
		//printf("可移动存储器设备!\r\n"); 
	}else if((*id).bInterfaceClass==0x03)
	{
		//printf("HID 设备!\r\n"); 
	}    
}
//获取到设备Manufacturer String
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	//printf("Manufacturer: %s\r\n",(char *)ManufacturerString);
}
//获取到设备Product String 
void USBH_USR_Product_String(void *ProductString)
{
	//printf("Product: %s\r\n",(char *)ProductString);  
}
//获取到设备SerialNum String 
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	//printf("Serial Number: %s\r\n",(char *)SerialNumString);    
} 
//设备USB枚举完成
void USBH_USR_EnumerationDone(void)
{ 
	//printf("设备枚举完成!\r\n\r\n");    
} 
//无法识别的USB设备
void USBH_USR_DeviceNotSupported(void)
{ 
	//printf("无法识别的USB设备!\r\n\r\n");    
}  
//等待用户输入按键,执行下一步操作
USBH_USR_Status USBH_USR_UserInput(void)
{ 
	//printf("跳过用户确认步骤!\r\n");
	bDeviceState=1;//USB设备已经连接成功
	return USBH_USR_RESP_OK;
} 
//USB接口电流过载
void USBH_USR_OverCurrentDetected (void)
{
	//printf("端口电流过大!!!\r\n");
}  
//重新初始化
void USBH_USR_DeInit(void)
{
	//printf("重新初始化!!!\r\n");
}
//无法恢复的错误!!  
void USBH_USR_UnrecoveredError (void)
{
	//printf("无法恢复的错误!!!\r\n\r\n");	
}
//////////////////////////////////////////////////////////////////////////////////////////
//下面两个函数,为ALIENTEK添加,以防止USB死机

//USB枚举状态死机检测,防止USB枚举失败导致的死机
//phost:USB_HOST结构体指针
//返回值:0,没有死机
//       1,死机了,外部必须重新启动USB连接.
u8 USBH_Check_EnumeDead(USBH_HOST *phost)
{
	static u16 errcnt=0;
	//这个状态,如果持续存在,则说明USB死机了.
	if(phost->gState==HOST_CTRL_XFER&&(phost->EnumState==ENUM_IDLE||phost->EnumState==ENUM_GET_FULL_DEV_DESC))
	{
		errcnt++;
		if(errcnt>2000)//死机了
		{ 
			errcnt=0;
			RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS,ENABLE);//USB OTG FS 复位
			delay_ms(5);
			RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS,DISABLE);	//复位结束  
			return 1;
		} 
	}else errcnt=0;
	return 0;
} 
//USB HID通信死机检测,防止USB通信死机(暂时仅针对:DTERR,即Data toggle error)
//pcore:USB_OTG_Core_dev_HANDLE结构体指针
//phidm:HID_Machine_TypeDef结构体指针 
//返回值:0,没有死机
//       1,死机了,外部必须重新启动USB连接.
u8 USBH_Check_HIDCommDead(USB_OTG_CORE_HANDLE *pcore,HID_Machine_TypeDef *phidm)
{
 	if(pcore->host.HC_Status[phidm->hc_num_in]==HC_DATATGLERR)//检测到DTERR错误,直接重启USB.
	{  
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//USB键盘鼠标数据处理

//鼠标初始化
void USR_MOUSE_Init	(void)
{
	USB_FIRST_PLUGIN_FLAG=1;//标记第一次插入
}
//键盘初始化
void  USR_KEYBRD_Init(void)
{ 
	USB_FIRST_PLUGIN_FLAG=1;//标记第一次插入
}

//零时数组,用于存放鼠标坐标/键盘输入内容(4.3屏,最大可以输入2016字节)

#pragma pack(push) //保存对齐状态 
#pragma pack(4)//设定为4字节对齐  
u8 tbuf[2017]; 
#pragma pack(pop) //保存对齐状态 

//USB鼠标数据处理
//data:USB鼠标数据结构体指针
void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data)
{  
	OS_ERR err;
	OSQPost(&HID_Q,
			(void *) data,
			1,
			OS_OPT_POST_FIFO,
			&err); 
} 


//USB键盘数据处理

void  USR_KEYBRD_ProcessData (uint8_t data)
{ 
	OS_ERR err;
	OSQPost(&HID_Q,
			(void *) data,
			0,
			OS_OPT_POST_FIFO,
			&err);
}

void USBH_HID_Reconnect(void)
{

	USBH_DeInit(&USB_OTG_Core_dev,&USB_Host);	
	USB_OTG_StopHost(&USB_OTG_Core_dev);		
	if(USB_Host.usr_cb->DeviceDisconnected)		
	{
		USB_Host.usr_cb->DeviceDisconnected(); 	
		USBH_DeInit(&USB_OTG_Core_dev, &USB_Host);
		USB_Host.usr_cb->DeInit();
		USB_Host.class_cb->DeInit(&USB_OTG_Core_dev,&USB_Host.device_prop);
	}
	USB_OTG_DisableGlobalInt(&USB_OTG_Core_dev);
	
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS,ENABLE);
	delay_ms(5);
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS,DISABLE); 

	memset(&USB_OTG_Core_dev,0,sizeof(USB_OTG_CORE_HANDLE));
	memset(&USB_Host,0,sizeof(USB_Host));

	USBH_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USB_Host,&HID_cb,&USR_Callbacks);  
}











