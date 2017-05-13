#include "led.h"

void led_init()
{
	gpio_init_pin(GPIOA,GPIO_Pin_6|GPIO_Pin_7,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_UP);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}
