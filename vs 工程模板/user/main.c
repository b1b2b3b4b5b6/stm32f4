#include "init.h"

int main(void)
{
  clock_init();
  gpio_init_pin(GPIOA,GPIO_Pin_6|GPIO_Pin_7,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_UP);
  while(1)
  {
	  
	delay_ms(1000);
    GPIO_SetBits(GPIOA,GPIO_Pin_6);
    GPIO_ResetBits(GPIOA,GPIO_Pin_7);
    delay_ms(1000);
    GPIO_SetBits(GPIOA,GPIO_Pin_7);
    GPIO_ResetBits(GPIOA,GPIO_Pin_6);
  }
}






