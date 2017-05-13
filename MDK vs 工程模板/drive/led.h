/*
 * led.h
 *
 * Created: 2015/12/5 1:05:58
 *  Author: God
 */ 


#ifndef		LED_H_
#define 	LED_H_

#include "init.h"

#define  ON 	1
#define  OFF 	0
#define LED_L(val)	(PAout(6)=val)
#define LED_R(val)	(PAout(7)=val)

void led_init(void);

#endif /* LED_H_ */
