/*
 * ili9325.h
 *
 * Created: 2015/10/10 0:21:04
 *  Author: God
 */


#ifndef ILI9325_H_
#define ILI9325_H_

#include "init.h"

//#define ASCII_FONT

#define X_MAX 239
#define Y_MAX 319


typedef struct
{
	vu16 reg;	//RW=1
	vu16 data;	//RW=0
} LCD_TypeDef;

typedef struct
{
	u16 width;
	u16 height;
	u8	state;	//0-横屏 1-竖屏
	u8  RGB;
}LCD_INFO;

extern  LCD_INFO lcd_info;
#define LCD_BASE        ((u32)(0x60000000 | 0x0007FFFE))
#define LCD	             ((LCD_TypeDef *) LCD_BASE)

void ili9325_init(void);
void ili9325_RGB(void);
void ili9325_BGR(void);
void ili9325_horizontal(void);
void ili9325_vertical(void);
void  ili9325_write_com(u16 com);
void ili9325_write_reg(u16 com,u16 data);	//向寄存器写入指令
u16 ili9325_read_reg(u16 reg);	//读寄存器
void ili9325_check_direction(u16 *real_x,u16 *real_y);

void ili9325_write_gram(const u8 *color);	//写入GRAM
u8 ili9325_read_point(u16 x,u16 y,u8 *color);
u8 ili9325_set_point(u16 x,u16 y);	//设置一个点
u8 ili9325_draw_point(u16 x,u16 y,const u8 *color);	//画一个点
u8 ili9325_set_window(u16 x1,u16 y1,u16 x2,u16 y2);//设定窗口
void ili9325_refresh(const u8 *color);//刷屏
u8 ili9325_draw_window(u16 x1,u16 y1,u16 x2,u16 y2,const u8 *color);//填充矩形
u8 ili9325_draw_line(u16 x1,u16 y1,u16 x2,u16 y2,const u8 *color);//画线
u8 ili9325_fill_window(u16 x,u16 y,u16 width,u16 height,const u8 *color  );

void ili9325_write_gram565(const u16 color);
u8 ili9325_draw_point565(u16 x,u16 y,const u16 color);
u8 ili9325_read_point565(u16 x,u16 y,u16 *color);
void ili9325_refresh565(const u16 color);
u8 ili9325_draw_window565(u16 x1,u16 y1,u16 x2,u16 y2,const u16 color);//填充矩形
u8 ili9325_draw_line565(u16 x1,u16 y1,u16 x2,u16 y2,const u16 color);
u8 ili9325_fill_window565(u16 x,u16 y,u16 width,u16 height,const u16 *color );

#ifdef ASCII_FONT

	u8 ili9325_show_char(u16 x,u16 y,u8 num,u8 size,const u8 *color,u8 mode);//mode=0填充，mode=1覆盖，左上角，左-右，上-下
	u8 ili9325_show_string(u16 x,u16 y,const u8 *string,u8 size,const u8 *color,u8 mode);
	u8 ili9325_show_char565(u16 x,u16 y,u8 num,u8 size,const u16 color,u8 mode);
	u8 ili9325_show_string565(u16 x,u16 y,const u8 *string,u8 size,const u16 color,u8 mode);
#endif
#endif /* ILI9325_H_ */
