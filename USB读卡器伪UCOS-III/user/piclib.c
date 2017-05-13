#include "piclib.h"

u8 piclib_fill_color(u16 x,u16 y,u16 width,u16 height,const u16 color)
{  
	return ili9325_draw_window565(x,y,x+width-1,y+height-1,color);	
}

u8 ai_load_picfile(FIL *file,u16 x,u16 y,u8 type)
{	
	u8	res;//����ֵ
	u8 temp;

	//��ʾ�Ŀ�ʼ�����
	picinfo.S_YOFF=y;
	picinfo.S_XOFF=x;
	//�ļ�������		 
	switch(type)
	{											  
		case T_BMP:
			res=stdbmp_decode(file); 				//����bmp	  	  
			break;
		case T_JPG:
		case T_JPEG:
			//res=jpg_decode(filename);				//����JPG/JPEG	  	  
			break;
		case T_GIF:
			//res=gif_decode(filename,x,y,width,height);	//����gif  	  
			break;
		default:
	 		res=PIC_FORMAT_ERR;  						//��ͼƬ��ʽ!!!  
			break;
	}  											   
	return res;
}























