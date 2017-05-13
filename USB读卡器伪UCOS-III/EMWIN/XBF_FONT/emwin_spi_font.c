#include "emwin_spi_font.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//STemwin XBF格式字体显示   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/8
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#if OS_SUPPORT
#include "includes.h"					//ucos 使用	  
#endif

//定义字体

GUI_FONT XBF16_Font;
GUI_FONT XBF24_Font;

GUI_XBF_DATA	XBF16_Data;
GUI_XBF_DATA	XBF24_Data;


//回调函数，用来获取字体数据
//参数：Off:		在XBF中偏移(位置)
//		NumBytes:	要读取的字节数
//		pVoid:	要读取的文件
//		pBuff:	读取到的数据的缓冲区
//返回值:0 成功，1 失败
static int _cbGetData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer) 
{


	//读取字体数据
	#if OS_SUPPORT

	#endif
	
	switch((u32)pVoid)
	{
	  case 16:
		W25QXX_Read(pBuffer,Off,NumBytes); 
		break;
		
	  case 24:
		W25QXX_Read(pBuffer,Off+2405408,NumBytes); 
		break;
	}
	
	#if OS_SUPPORT

	#endif
	
	return 0; 
}


//创建XBF16字体，共EMWIN使用
//fxpath:XBF字体文件路径
//返回值:0，成功；1，失败
u8 Create_XBF16(u8 *fxpath) 
{
	GUI_XBF_CreateFont(	&XBF16_Font,    //指向GUI_FONT结构
						&XBF16_Data, 	//指向GUI_XBF_DATA结构
						GUI_XBF_TYPE_PROP_AA2_EXT,//要创建的字体类型
						_cbGetData,   	//回调函数
						(void *)16);  //窗体给回调函数_cbGetData的参数
	return 0;
}

u8 Create_XBF24(u8 *fxpath) 
{
	GUI_XBF_CreateFont(	&XBF24_Font,    //指向GUI_FONT结构
						&XBF24_Data, 	//指向GUI_XBF_DATA结构
						GUI_XBF_TYPE_PROP_AA2_EXT,//要创建的字体类型
						_cbGetData,   	//回调函数
						(void *)24);  //窗体给回调函数_cbGetData的参数
	return 0;
}

void emwin_font_load()
{	
	FIL *file1,*file2;
	u32 br,offset;
	u8 *f1,*f2;
	OS_ERR err;
	u8 res;
	u32 n,i;
	printf("FONT_INIT_ING_ _ _ _\n");

	f1=mymalloc(4096);
	f2=mymalloc(4096);
	file1=mymalloc(sizeof(FIL));
	file2=mymalloc(sizeof(FIL));
	if((f1==NULL)||(f2==NULL)) 
	{
	  myfree(f1);
	  myfree(f2);
	  myfree(file1);
	  myfree(file2);
	  printf("can't get memory\n");
	  return ;
	}

	printf("Erase_Chip_Ing- - - \n");
	W25QXX_Erase_Chip();
	printf("Erase_Chip_Done!\n");
	res=f_open(file1,"0:/EMWIN_FONT/16.xbf",FA_READ);	//打开字库文件
	if(res!=FR_OK)	
	{
	  myfree(f1);
	  myfree(f2);
	  printf("can't open file\n");
	  return ;
	}	
	res=f_open(file2,"0:/EMWIN_FONT/24.xbf",FA_READ);	//打开字库文件
	if(res!=FR_OK)	
	{
		f_close(file1);
		myfree(f1);
		myfree(f2);
		printf("can't open file\n");
		return ;
	}	
	OSSchedLock(&err);
	offset=0;
	i=0;
	do
	{
		f_lseek(file1,offset);
		res=f_read(file1,f1,4096,&br);
		usart1_send_buff(f1,br);	
		W25QXX_Write_NoCheck(f1,offset,(u16)br);
		W25QXX_Read(f2,offset,(u16)br);
		for(n=0;n<br;n++)
		{
			if(f1[n]!=f2[n]) i++;
		}
		offset+=br;
	}while(br!=0);
	f_close(file1);
	OSSchedUnlock(&err);

	printf("XBF16Done!\n");
	printf("offset:%d,err:%d\n",offset,i);

	OSSchedLock(&err);
	i=0;
	do
	{
		f_lseek(file2,offset-2405408);
		res=f_read(file2,f1,4096,&br);
		usart1_send_buff(f1,br);	
		W25QXX_Write_NoCheck(f1,offset,(u16)br);
		W25QXX_Read(f2,offset,(u16)br);
		for(n=0;n<br;n++)
		{
			if(f1[n]!=f2[n]) i++;
		}
		offset+=br;
	}while(br!=0);	
	f_close(file2);		

	OSSchedUnlock(&err);

	myfree(f1);
	myfree(f2);
	myfree(file1);
	myfree(file2);
	printf("XBF24Done!\n");
	printf("offset:%d,err:%d\n",offset,i);
	OSSemSet(&KeySem_0,0,&err);
}