/*
 * spi.h
 *
 * Created: 2015/10/25 0:03:30
 *  Author: God
 */ 


#ifndef SPI1_H_
#define SPI1_H_

#include "init.h"


#define SPI_SPEED			SPI_BaudRatePrescaler_256
#define	SPIX  SPI1			  	    													  
void 	spi_init(void);	//spi初始化
void 	spi_gpio_init(void);	//spi引脚初始化
u16		spi_rw(u16 byte);	//写or读一字节
uchar 	spi_write_reg(uchar reg,uchar value);		//写寄存器
uchar 	spi_read_reg(uchar reg);					//读寄存器
uchar 	spi_write_buf(uchar reg,uchar *Buf,u32 n);	//写缓冲区
uchar 	spi_read_buf(uchar reg,uchar *Buf,u32 n);		//读缓冲区
void 	spi_speed(u32 SPI_BaudRatePrescaler);

 				  	    													  
#endif /* SPI_H_ */
