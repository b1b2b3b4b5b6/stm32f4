#include "piclib.h"

u8 piclib_fill_color(u16 x,u16 y,u16 width,u16 height,const u16 color)
{  
	return ili9325_draw_window565(x,y,x+width-1,y+height-1,color);	
}

u8 ai_load_picfile(FIL *file,u16 x,u16 y,u8 type)
{	
	u8	res;//返回值
	u8 temp;

	//显示的开始坐标点
	picinfo.S_YOFF=y;
	picinfo.S_XOFF=x;
	//文件名传递		 
	switch(type)
	{											  
		case T_BMP:
			res=stdbmp_decode(file); 				//解码bmp	  	  
			break;
		case T_JPG:
		case T_JPEG:
			//res=jpg_decode(filename);				//解码JPG/JPEG	  	  
			break;
		case T_GIF:
			//res=gif_decode(filename,x,y,width,height);	//解码gif  	  
			break;
		default:
	 		res=PIC_FORMAT_ERR;  						//非图片格式!!!  
			break;
	}  											   
	return res;
}























