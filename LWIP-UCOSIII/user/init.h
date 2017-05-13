/*
 * init.h
 *
 * Created: 2015/10/6 17:06:27
 *  Author: God
 */ 


#ifndef INIT_H_
#define INIT_H_
#define uchar u8
#define uint u16

#include "stm32f4xx.h" 
#include "sdio_sdcard.h"
#include "exfuns.h"
#include "sys.h"
#include "delay.h"
#include "string.h"
#include "integer.h"
#include "arm_math.h"
#include "includes.h"
#include "os_app_hooks.h"
#include "GUI.h"
#include "usart1.h"
#include "OSTmr.h"
#include "key.h"
	 
extern OS_TCB EmwindemoTaskTCB;

void clock_init(void);

void gpio_init_pin(GPIO_TypeDef* GPIOx, u16 GPIO_Pin,GPIOMode_TypeDef MODE,GPIOOType_TypeDef OType,GPIOPuPd_TypeDef PUPDR);



#endif /* INIT_H_ */
