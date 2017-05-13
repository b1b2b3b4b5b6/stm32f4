#include "emwin_spi_font.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemwin XBF��ʽ������ʾ   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/8
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#if OS_SUPPORT
#include "includes.h"					//ucos ʹ��	  
#endif

//��������

GUI_FONT XBF16_Font;
GUI_FONT XBF24_Font;

GUI_XBF_DATA	XBF16_Data;
GUI_XBF_DATA	XBF24_Data;


//�ص�������������ȡ��������
//������Off:		��XBF��ƫ��(λ��)
//		NumBytes:	Ҫ��ȡ���ֽ���
//		pVoid:	Ҫ��ȡ���ļ�
//		pBuff:	��ȡ�������ݵĻ�����
//����ֵ:0 �ɹ���1 ʧ��
static int _cbGetData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer) 
{


	//��ȡ��������
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


//����XBF16���壬��EMWINʹ��
//fxpath:XBF�����ļ�·��
//����ֵ:0���ɹ���1��ʧ��
u8 Create_XBF16(u8 *fxpath) 
{
	GUI_XBF_CreateFont(	&XBF16_Font,    //ָ��GUI_FONT�ṹ
						&XBF16_Data, 	//ָ��GUI_XBF_DATA�ṹ
						GUI_XBF_TYPE_PROP_AA2_EXT,//Ҫ��������������
						_cbGetData,   	//�ص�����
						(void *)16);  //������ص�����_cbGetData�Ĳ���
	return 0;
}

u8 Create_XBF24(u8 *fxpath) 
{
	GUI_XBF_CreateFont(	&XBF24_Font,    //ָ��GUI_FONT�ṹ
						&XBF24_Data, 	//ָ��GUI_XBF_DATA�ṹ
						GUI_XBF_TYPE_PROP_AA2_EXT,//Ҫ��������������
						_cbGetData,   	//�ص�����
						(void *)24);  //������ص�����_cbGetData�Ĳ���
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
	res=f_open(file1,"0:/EMWIN_FONT/16.xbf",FA_READ);	//���ֿ��ļ�
	if(res!=FR_OK)	
	{
	  myfree(f1);
	  myfree(f2);
	  printf("can't open file\n");
	  return ;
	}	
	res=f_open(file2,"0:/EMWIN_FONT/24.xbf",FA_READ);	//���ֿ��ļ�
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