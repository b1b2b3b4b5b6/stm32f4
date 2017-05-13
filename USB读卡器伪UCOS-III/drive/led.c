#include "led.h"

void led_init()
{
	gpio_init_pin(GPIOA,GPIO_Pin_6|GPIO_Pin_7,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_UP);
	LED_L(OFF);
	LED_R(OFF);
}
