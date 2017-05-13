/*
 * xpt2046.h
 *
 * Created: 2015/10/7 18:57:00
 *  Author: God
 */


#ifndef __XPT2046_H
#define __XPT2046_H

#include "init.h"

#define XPT_MAX 2

	#define XPT_CS_OUT			gpio_init_pin(GPIOB,GPIO_Pin_12,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_UP);
	#define XPT_CS_SET			GPIO_SetBits(GPIOB,GPIO_Pin_12)
	#define XPT_CS_RESET		GPIO_ResetBits(GPIOB,GPIO_Pin_12)

	#define XPT_SCK_OUT		gpio_init_pin(GPIOB,GPIO_Pin_13,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_UP);
	#define XPT_SCK_SET			GPIO_SetBits(GPIOB,GPIO_Pin_13)
	#define XPT_SCK_RESET		GPIO_ResetBits(GPIOB,GPIO_Pin_13)

	#define XPT_MISO_IN		gpio_init_pin(GPIOB,GPIO_Pin_14,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_UP);
	#define XPT_MISO_R			GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)

	#define XPT_MOSI_OUT	gpio_init_pin(GPIOB,GPIO_Pin_15,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_UP);
	#define XPT_MOSI_SET		GPIO_SetBits(GPIOB,GPIO_Pin_15)
	#define XPT_MOSI_RESET	GPIO_ResetBits(GPIOB,GPIO_Pin_15)


	#define XPT_PENIRQ_IN		gpio_init_pin(GPIOC,GPIO_Pin_5,GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_UP);
	#define XPT_PENIRQ_R		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)

void xpt2046_init(void);
u8 xpt2046_xy(u16 *p);
u8 xpt2046_read(u16 *p,u8 times);
u8 xpt_spi_rw(u8 byte);
void xpt_spi_gpio_init(void);

#endif /* XPT2046_H_ */
