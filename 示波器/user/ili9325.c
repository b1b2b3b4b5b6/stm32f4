/*
 * ili9325.c
 *
 * Created: 2015/10/10 0:20:50
 *  Author: God
 */

#include "ili9325.h"

LCD_INFO lcd_info;

void ili9325_RGB(void)
{
	ili9325_write_reg(0x0003,ili9325_read_reg(0x0003)|0x1000);
	lcd_info.RGB=1;
}
void ili9325_BGR(void)
{
	ili9325_write_reg(0x0003,ili9325_read_reg(0x0003)&(~0x1000));
	lcd_info.RGB=0;
}
void ili9325_horizontal(void)
{
	ili9325_write_reg(0x0003,ili9325_read_reg(0x0003)|0x0008);
	lcd_info.state=0;
	lcd_info.width=Y_MAX;
	lcd_info.height=X_MAX;
}
void ili9325_vertical(void)
{
	ili9325_write_reg(0x0003,ili9325_read_reg(0x0003)&(~0x0008));
	lcd_info.state=1;
	lcd_info.width=X_MAX;
	lcd_info.height=Y_MAX;
}
void ili9325_write_com(u16 com)
{
	LCD->reg=com;
}

void ili9325_init(void)	//初始化
{
	delay_ms(500);
	//根据不同晶振速度可以调整延时，保障稳定显示
	ili9325_write_reg(0x0001,0x0100);
	ili9325_write_reg(0x0002,0x0700);
	ili9325_write_reg(0x0003,0x1030);//竖屏RGB
	ili9325_write_reg(0x0004,0x0000);
	ili9325_write_reg(0x0008,0x0207);
	ili9325_write_reg(0x0009,0x0000);
	ili9325_write_reg(0x000A,0x0000);
	ili9325_write_reg(0x000C,0x0000);
	ili9325_write_reg(0x000D,0x0000);
	ili9325_write_reg(0x000F,0x0000);
	//power on sequence VGHVGL
	ili9325_write_reg(0x0010,0x0000);
	ili9325_write_reg(0x0011,0x0007);
	ili9325_write_reg(0x0012,0x0000);
	ili9325_write_reg(0x0013,0x0000);
	//vgh
	ili9325_write_reg(0x0010,0x1290);
	ili9325_write_reg(0x0011,0x0227);
	//delayms(100);
	//vregiout
	ili9325_write_reg(0x0012,0x001d); //0x001b
	//delayms(100);
	//vom amplitude
	ili9325_write_reg(0x0013,0x1500);
	//delayms(100);
	//vom H
	ili9325_write_reg(0x0029,0x0018);
	ili9325_write_reg(0x002B,0x000D);

	//gamma
	ili9325_write_reg(0x0030,0x0004);
	ili9325_write_reg(0x0031,0x0307);
	ili9325_write_reg(0x0032,0x0002);// 0006
	ili9325_write_reg(0x0035,0x0206);
	ili9325_write_reg(0x0036,0x0408);
	ili9325_write_reg(0x0037,0x0507);
	ili9325_write_reg(0x0038,0x0204);//0200
	ili9325_write_reg(0x0039,0x0707);
	ili9325_write_reg(0x003C,0x0405);// 0504
	ili9325_write_reg(0x003D,0x0F02);
	//ram
	ili9325_write_reg(0x0050,0x0000);
	ili9325_write_reg(0x0051,0x00EF);
	ili9325_write_reg(0x0052,0x0000);
	ili9325_write_reg(0x0053,0x013F);
	ili9325_write_reg(0x0060,0xA700);
	ili9325_write_reg(0x0061,0x0001);
	ili9325_write_reg(0x006A,0x0000);
	//
	ili9325_write_reg(0x0080,0x0000);
	ili9325_write_reg(0x0081,0x0000);
	ili9325_write_reg(0x0082,0x0000);
	ili9325_write_reg(0x0083,0x0000);
	ili9325_write_reg(0x0084,0x0000);
	ili9325_write_reg(0x0085,0x0000);
	//
	ili9325_write_reg(0x0090,0x0010);
	ili9325_write_reg(0x0092,0x0600);
	ili9325_write_reg(0x0093,0x0003);
	ili9325_write_reg(0x0095,0x0110);
	ili9325_write_reg(0x0097,0x0000);
	ili9325_write_reg(0x0098,0x0000);
	ili9325_write_reg(0x0007,0x0133);
	ili9325_vertical();
	ili9325_RGB();
}

u16 ili9325_read_reg(u16 reg)//读寄存器
{
  LCD->reg =reg;
  return (LCD->data);
}
void ili9325_write_reg(u16 com,u16 data)//向寄存器写入指令
{


  LCD->reg =com;

  LCD->data = data;

}
u8 ili9325_set_window(u16 x1,u16 y1,u16 x2,u16 y2)////设定窗口
{
	if ((x2>X_MAX)||(y2>Y_MAX))
	{
		return 0;
	}
	if ((x1>x2)||(y1>y2))
	{
		return 0;
	}
	ili9325_write_reg(0x20,x1);	 //设置X坐标位置
	ili9325_write_reg(0x21,y1);//设置Y坐标位置
	ili9325_write_reg(0x50,x1);	 //开始X
	ili9325_write_reg(0x52,y1);	  //开始Y
	ili9325_write_reg(0x51,x2);	//结束X
	ili9325_write_reg(0x53,y2);	//结束Y
	ili9325_write_com(0x22);
	return 1;
}
u8 ili9325_set_point(u16 x,u16 y)
{
	if ((x>X_MAX)||(y>Y_MAX))
	{
		return 0;
	}
	ili9325_write_reg(0x0020,x);
	ili9325_write_reg(0x0021,y);
	ili9325_write_com(0x22);
	return 1;
}
void ili9325_check_direction(u16 *real_x,u16 *real_y)
{
	u16 temp;
	if(lcd_info.state==0)
	{
		temp=*real_x;
		*real_x=X_MAX-*real_y;
		*real_y=temp;
	}
}

	void ili9325_write_gram(const u8 *color)//写GRAM
	{
		u16 color565,temp;

		temp=*color&0xf8;
		color565=temp<<8;

		temp=*(color+1)&0xfc;
		color565|=temp<<3;

		color565|=(*(color+2)&0xf8)>>3;
		LCD->data=color565;
	}
	u8 ili9325_draw_point(u16 x,u16 y,const u8 *color)
	{
	  	ili9325_check_direction(&x,&y);
		if(!ili9325_set_point(x,y)) return 0;
		ili9325_write_gram(color);
		return 1;
	}
	u8 ili9325_read_point(u16 x,u16 y,u8 *color)
	{
		u16 temp;
		ili9325_check_direction(&x,&y);
		if (!ili9325_set_point(x,y))	return 0;
		ili9325_read_reg(0x22);
		temp=ili9325_read_reg(0x22);
		if(lcd_info.RGB==0)
		{
		  *color=(temp&0xf800)>>8;
		  *(color+1)=(temp&0x07e0)>>3;
		  *(color+2)=(temp&0x001f)<<3;
		}
		else
		{
		  *(color+2)=(temp&0x001f)<<3;	
		  *color=(temp&0xf800)>>8;
		  *color=(temp&0xf800)>>8;
	  
		}
		return 1;
	}
	void ili9325_refresh(const u8 *color)//设定屏幕颜色
	{
		ili9325_draw_window(0,0,X_MAX,Y_MAX,color);
	}
	u8 ili9325_draw_window(u16 x1,u16 y1,u16 x2,u16 y2,const u8 *color)//填充矩形
	{
		u32 num;
		u16 temp;
		ili9325_check_direction(&x1,&y1);
		ili9325_check_direction(&x2,&y2);
		if(x1>x2)
		{
			temp=x1;
			x1=x2;
			x2=temp;
		}
		if(y1>y2)
		{
			temp=y1;
			y1=y2;
			y2=temp;
		}
		if(!ili9325_set_window(x1,y1,x2,y2))
		{
			return 0;
		}
		for (num=(x2-x1+1)*(y2-y1+1);num>0;num--)
		{
			ili9325_write_gram(color);
		}
		ili9325_set_window(0,0,X_MAX,Y_MAX);
		return 1;
	}
	u8 ili9325_draw_line(u16 x1,u16 y1,u16 x2,u16 y2,const u8 *color)//画线
	{
		s16 dx,dy;
		s16 f;
		ili9325_check_direction(&x1,&y1);
		ili9325_check_direction(&x2,&y2);
		ili9325_set_window(0,0,X_MAX,Y_MAX);
		if ((x1>X_MAX)||(y1>Y_MAX)||(x2>X_MAX)||(y2>Y_MAX))
		{
			return 0;
		}
		dx=x2-x1;
		dy=y2-y1;
		if (dx==0)
		{
			if (dy>=0)
			{
				for(;y1<=y2;y1++)
				{
					ili9325_draw_point(x1,y1,color);
				}
			}
			else
			{
				for(;y2<=y1;y2++)
				{
					ili9325_draw_point(x1,y2,color);
				}
			}
			return 1;
		}

		if (dy==0)
		{
			if (dx>=0)
			{
				for(;x1<=x2;x1++)
				{
					ili9325_draw_point(x1,y1,color);
				}
			}
			else
			{
				for(;x2<=x1;x2++)
				{
					ili9325_draw_point(x2,y1,color);
				}
			}
			return 1;
		}
		ili9325_draw_point(x1,y1,color);
		f=0;
		if ((dx>0)&&(dy>0))
		{
			while(!((x1==x2)&&(y1==y2)))
			{
				if (f>=0)
				{
					x1++;
					f-=dy;
				}
				else
				{
					if (f<0)
					{
						y1++;
						f+=dx;
					}
				}
				ili9325_draw_point(x1,y1,color);
			}
		}
		if ((dx>0)&&(dy<0))
		{
			while(!((x1==x2)&&(y1==y2)))
			{
				if (f>=0)
				{
					x1++;
					f+=dy;
				}
				else
				{
					if (f<0)
					{
						y1--;
						f+=dx;
					}
				}
				ili9325_draw_point(x1,y1,color);
			}
		}
		if ((dx<0)&&(dy>0))
		{
			while(!((x1==x2)&&(y1==y2)))
			{
				if (f>=0)
				{
					x1--;
					f-=dy;
				}
				else
				{
					if (f<0)
					{
						y1++;
						f-=dx;
					}
				}
				ili9325_draw_point(x1,y1,color);
			}
		}
		if ((dx<0)&&(dy<0))
		{
			while(!((x1==x2)&&(y1==y2)))
			{
				if (f>=0)
				{
					x1--;
					f+=dy;
				}
				else
				{
					if (f<0)
					{
						y1--;
						f-=dx;
					}
				}
				ili9325_draw_point(x1,y1,color);
			}
		}
		return 1;
	}
	u8 ili9325_fill_window(u16 x,u16 y,u16 width,u16 height,const u8 *color )
	{
		u16 n;
		u8 *temp;
		temp=(u8 *)color;
		if(!ili9325_set_window(x,y,x+width-1,y+height-1)) return 0;
		for (n=width*height;n>0;n--)
		{
			ili9325_write_gram(temp);
			temp+=3;
		}
		ili9325_set_window(0,0,X_MAX,Y_MAX);
		return 1;
	}



	void ili9325_write_gram565(const u16 color)
	{
		LCD->data=color;
	}
	u8 ili9325_draw_point565(u16 x,u16 y,const u16 color)
	{
		ili9325_check_direction(&x,&y);
		if (!(ili9325_set_point(x,y)))
		{
			return 0;
		}
		ili9325_write_gram565(color);
		return 1;
	}
	u8 ili9325_read_point565(u16 x,u16 y,u16 *color)
	{
	  	u16 temp;
		ili9325_check_direction(&x,&y);
		if (!ili9325_set_point(x,y))	return 0;
		ili9325_read_reg(0x22);
		temp=ili9325_read_reg(0x22);
		if(lcd_info.RGB==0)	*color=temp;
		else 
		{
			*color=temp&(0x07e0);
			*color=*color|(temp>>11)|(temp<<11);
		}
		return 1;
	}
	void ili9325_refresh565(const u16 color)//设定屏幕颜色
	{
		ili9325_draw_window565(0,0,X_MAX,Y_MAX,color);
	}
	u8 ili9325_draw_window565(u16 x1,u16 y1,u16 x2,u16 y2,const u16 color)//填充矩形
	{
		u32 num;
		u16 temp;
		ili9325_check_direction(&x1,&y1);
		ili9325_check_direction(&x2,&y2);
		if(x1>x2)
		{
			temp=x1;
			x1=x2;
			x2=temp;
		}
		if(y1>y2)
		{
			temp=y1;
			y1=y2;
			y2=temp;
		}
		if(!ili9325_set_window(x1,y1,x2,y2))
		{
			return 0;
		}
		for (num=(x2-x1+1)*(y2-y1+1);num>0;num--)
		{
			ili9325_write_gram565(color);
		}
		ili9325_set_window(0,0,X_MAX,Y_MAX);
		return 1;
	}
	u8 ili9325_draw_line565(u16 x1,u16 y1,u16 x2,u16 y2,const u16 color)//画线
	{
		s16 dx,dy;
		s16 f;
		ili9325_check_direction(&x1,&y1);
		ili9325_check_direction(&x2,&y2);
		ili9325_set_window(0,0,X_MAX,Y_MAX);
		if ((x1>X_MAX)||(y1>Y_MAX)||(x2>X_MAX)||(y2>Y_MAX))
		{
			return 0;
		}
		dx=x2-x1;
		dy=y2-y1;
		if (dx==0)
		{
			if (dy>=0)
			{
				for(;y1<=y2;y1++)
				{
					ili9325_draw_point565(x1,y1,color);
				}
			}
			else
			{
				for(;y2<=y1;y2++)
				{
					ili9325_draw_point565(x1,y2,color);
				}
			}
			return 1;
		}

		if (dy==0)
		{
			if (dx>=0)
			{
				for(;x1<=x2;x1++)
				{
					ili9325_draw_point565(x1,y1,color);
				}
			}
			else
			{
				for(;x2<=x1;x2++)
				{
					ili9325_draw_point565(x2,y1,color);
				}
			}
			return 1;
		}
		ili9325_draw_point565(x1,y1,color);
		f=0;
		if ((dx>0)&&(dy>0))
		{
			while(!((x1==x2)&&(y1==y2)))
			{
				if (f>=0)
				{
					x1++;
					f-=dy;
				}
				else
				{
					if (f<0)
					{
						y1++;
						f+=dx;
					}
				}
				ili9325_draw_point565(x1,y1,color);
			}
		}
		if ((dx>0)&&(dy<0))
		{
			while(!((x1==x2)&&(y1==y2)))
			{
				if (f>=0)
				{
					x1++;
					f+=dy;
				}
				else
				{
					if (f<0)
					{
						y1--;
						f+=dx;
					}
				}
				ili9325_draw_point565(x1,y1,color);
			}
		}
		if ((dx<0)&&(dy>0))
		{
			while(!((x1==x2)&&(y1==y2)))
			{
				if (f>=0)
				{
					x1--;
					f-=dy;
				}
				else
				{
					if (f<0)
					{
						y1++;
						f-=dx;
					}
				}
				ili9325_draw_point565(x1,y1,color);
			}
		}
		if ((dx<0)&&(dy<0))
		{
			while(!((x1==x2)&&(y1==y2)))
			{
				if (f>=0)
				{
					x1--;
					f+=dy;
				}
				else
				{
					if (f<0)
					{
						y1--;
						f-=dx;
					}
				}
				ili9325_draw_point565(x1,y1,color);
			}
		}
		return 1;
	}
	u8	ili9325_fill_window565(u16 x,u16 y,u16 width,u16 height,const u16 *color )
	{
		u16 n;
		u16 *temp;
		temp=(u16 *)color;
		if(!ili9325_set_window(x,y,x+width-1,y+height-1)) return 0;
		for (n=width*height;n>0;n--)
		{
			ili9325_write_gram565(*temp);
			temp++;
		}
		ili9325_set_window(0,0,X_MAX,Y_MAX);
		return 1;
	}


#ifdef ASCII_FONT
	#include "ASCII_FONT.H"
	u8	ili9325_show_char(u16 x,u16 y,u8 num,u8 size,const u8 *color,u8 mode)
	{
	    u8 temp,t1,t;
		const u8 black[3]={0,0,0};
	    u16 y0=y;
	    u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	    num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	    for(t=0;t<csize;t++)
	    {
		    if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		    else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		    //else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		    else return 0;								//没有的字库
		    for(t1=0;t1<8;t1++)
		    {

			    if(temp&0x80)
					{
						if(!ili9325_draw_point(x,y,color)) return 0;
					}
			    else
					if(mode==0)
					{
						if(!ili9325_draw_point(x,y,black)) return 0;
					}
			    temp<<=1;
			    y++;
					if((y-y0)==size)
					{
						y=y0;
						x++;
						break;
					}
		    }
	    }
		return 1;
	}
	u8 ili9325_show_string(u16 x,u16 y,const u8 *string,u8 size,const u8 *color,u8 mode)
	{
		while (*string!=0)
		{
			if(!ili9325_show_char(x,y,*string,size,color,mode)) return 0;
			string++;
			x+=size/2;
		}
		return 1;
	}
u8	ili9325_show_char565(u16 x,u16 y,u8 num,u8 size,const u16 color,u8 mode)
	{
	    u8 temp,t1,t;
		const u16 black=0;
	    u16 y0=y;
	    u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	    num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	    for(t=0;t<csize;t++)
	    {
		    if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		    else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		    //else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		    else return 0;								//没有的字库
		    for(t1=0;t1<8;t1++)
		    {

			    if(temp&0x80)
					{
						if(!ili9325_draw_point565(x,y,color)) return 0;
					}
			    else
					if(mode==0)
					{
						if(!ili9325_draw_point565(x,y,black)) return 0;
					}
			    temp<<=1;
			    y++;
					if((y-y0)==size)
					{
						y=y0;
						x++;
						break;
					}
		    }
	    }
		return 1;
	}

	u8 ili9325_show_string565(u16 x,u16 y,const u8 *string,u8 size,const u16 color,u8 mode)
	{
		while (*string!=0)
		{
			if(!ili9325_show_char565(x,y,*string,size,color,mode)) return 0;
			string++;
			x+=size/2;
		}
		return 1;
	}
#endif