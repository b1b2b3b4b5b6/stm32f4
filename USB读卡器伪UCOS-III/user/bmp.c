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
	u8 *databuf;    		//数据读取存放地址
 	u16 readlen=BMP_DBUF_SIZE;//一次从SD卡读取的字节数长度
	u8 biCompression=0;		//记录压缩方式

	u16 rowlen;	  		 	//水平方向字节数
	BITMAPINFO *pbmp;		//临时指针

	databuf=(u8*)mymalloc(readlen);		//开辟readlen字节的内存区域
	if(databuf==NULL)	return 1;	//内存申请失败.
	f_bmp=(FIL *)mymalloc(sizeof(FIL));	//开辟FIL字节的内存区域
	if(f_bmp==NULL)								//内存申请失败.
	{
		myfree(databuf);
		return 1;
	}
	res=f_open(f_bmp,(const TCHAR*)filename,FA_READ);//打开文件
	if (res==0)
	{
		f_read(f_bmp,databuf,0x36,(UINT*)&br);	//读出readlen个字节
		pbmp=(BITMAPINFO*)databuf;					//得到BMP的头部信息
		count=pbmp->bmfHeader.bfOffBits;        	//数据偏移,得到数据段的开始地址
		color_byte=pbmp->bmiHeader.biBitCount/8;	//彩色位 16/24/32
		biCompression=pbmp->bmiHeader.biCompression;//压缩方式
		ImgHeight=pbmp->bmiHeader.biHeight;	//得到图片高度
		ImgWidth=pbmp->bmiHeader.biWidth;  	//得到图片宽度
		//水平像素必须是4的倍数!!
		if((ImgWidth*color_byte)%4)	rowlen=((ImgWidth*color_byte)/4+1)*4;
		else rowlen=ImgWidth*color_byte;
		//开始解码BMP
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
	f_close(f_bmp);//关闭文件
	myfree(databuf);
	myfree(f_bmp);

	return res;		//BMP显示结束.
}

