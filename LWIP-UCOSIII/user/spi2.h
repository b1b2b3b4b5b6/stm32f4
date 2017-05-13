#ifndef SPI2_H
#define SPI2_H

#include "init.h"

#define SPI2_SPEED			SPI_BaudRatePrescaler_256
#define	SPIX2  SPI2			  	    													  
void 	spi2_init(void);	//spi³õÊ¼»¯
void 	spi2_gpio_init(void);	//spiÒý½Å³õÊ¼»¯
u16		spi2_rw(u16 byte);	//Ð´or¶ÁÒ»×Ö½Ú
uchar 	spi2_write_reg(uchar reg,uchar value);		//Ð´¼Ä´æÆ÷
uchar 	spi2_read_reg(uchar reg);					//¶Á¼Ä´æÆ÷
uchar 	spi2_write_buf(uchar reg,uchar *Buf,u32 n);	//Ð´»º³åÇø
uchar 	spi2_read_buf(uchar reg,uchar *Buf,u32 n);		//¶Á»º³åÇø
void 	spi2_speed(u32 SPI_BaudRatePrescaler);



#endif