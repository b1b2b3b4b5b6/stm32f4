#ifndef EMWIN_SPI_FONT_H
#define EMWIN_SPI_FONT_H

#include "w25qxx.h"
#include "init.h"
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

extern GUI_FONT XBF16_Font;
extern GUI_FONT XBF24_Font;



u8 Create_XBF16(u8 *fxpath); 
u8 Create_XBF24(u8 *fxpath); 
void emwin_font_load();

#endif
