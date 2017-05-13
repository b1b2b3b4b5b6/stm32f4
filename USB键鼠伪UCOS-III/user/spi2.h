#ifndef SPI2_H
#define SPI2_H

#include "init.h"

#define SPI2_SPEED			SPI_BaudRatePrescaler_256
#define	SPIX2  SPI2			  	    													  
void 	spi2_init(void);	//spi��ʼ��
void 	spi2_gpio_init(void);	//spi���ų�ʼ��
u16		spi2_rw(u16 byte);	//дor��һ�ֽ�
uchar 	spi2_write_reg(uchar reg,uchar value);		//д�Ĵ���
uchar 	spi2_read_reg(uchar reg);					//���Ĵ���
uchar 	spi2_write_buf(uchar reg,uchar *Buf,u32 n);	//д������
uchar 	spi2_read_buf(uchar reg,uchar *Buf,u32 n);		//��������
void 	spi2_speed(u32 SPI_BaudRatePrescaler);



#endif