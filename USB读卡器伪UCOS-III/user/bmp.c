#include "bmp.h"


u8 bmp_decode(const u8 *filename,u16 x_off,u16 y_off)
{
	u16 br;
    vu16 count;
	u8   color_byte;
	u16 x ,y;
	vu16 ImgWidth,ImgHeight;
	u8 res;
	FIL *f_bmp;
	u8 *databuf;    		//���ݶ�ȡ��ŵ�ַ
 	u16 readlen=BMP_DBUF_SIZE;//һ�δ�SD����ȡ���ֽ�������
	u8 biCompression=0;		//��¼ѹ����ʽ

	u16 rowlen;	  		 	//ˮƽ�����ֽ���
	BITMAPINFO *pbmp;		//��ʱָ��

	databuf=(u8*)mymalloc(readlen);		//����readlen�ֽڵ��ڴ�����
	if(databuf==NULL)	return 1;	//�ڴ�����ʧ��.
	f_bmp=(FIL *)mymalloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ�����
	if(f_bmp==NULL)								//�ڴ�����ʧ��.
	{
		myfree(databuf);
		return 1;
	}
	res=f_open(f_bmp,(const TCHAR*)filename,FA_READ);//���ļ�
	if (res==0)
	{
		f_read(f_bmp,databuf,0x36,(UINT*)&br);	//����readlen���ֽ�
		pbmp=(BITMAPINFO*)databuf;					//�õ�BMP��ͷ����Ϣ
		count=pbmp->bmfHeader.bfOffBits;        	//����ƫ��,�õ����ݶεĿ�ʼ��ַ
		color_byte=pbmp->bmiHeader.biBitCount/8;	//��ɫλ 16/24/32
		biCompression=pbmp->bmiHeader.biCompression;//ѹ����ʽ
		ImgHeight=pbmp->bmiHeader.biHeight;	//�õ�ͼƬ�߶�
		ImgWidth=pbmp->bmiHeader.biWidth;  	//�õ�ͼƬ���
		//ˮƽ���ر�����4�ı���!!
		if((ImgWidth*color_byte)%4)	rowlen=((ImgWidth*color_byte)/4+1)*4;
		else rowlen=ImgWidth*color_byte;
		//��ʼ����BMP
		if (count!=0x36) f_read(f_bmp,databuf,count-0x36,(UINT*)&br);

		x=1+x_off-1 ;
		y=ImgHeight+y_off-1;

		if (color_byte==3)
		{
			ili9325_BGR();
			for (count=ImgHeight;count>0;count--)
			{
				f_read(f_bmp,databuf,rowlen,(UINT*)&br);
				ili9325_fill_window(x-1,y-1,ImgWidth,1,databuf);
				y--;
			}
			ili9325_RGB();
		}
	}
	f_close(f_bmp);//�ر��ļ�
	myfree(databuf);
	myfree(f_bmp);

	return res;		//BMP��ʾ����.
}

