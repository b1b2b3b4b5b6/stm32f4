
#ifndef		LED_H_
#define 	LED_H_

#include "init.h"

#define  ON 	0
#define  OFF 	1
#define LED_L(val)	(PAout(6)=val)
#define LED_R(val)	(PAout(7)=val)

void led_init();

#endif /* LED_H_ */
