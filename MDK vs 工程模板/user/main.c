#include "init.h"



//ALIENTEK 探索者STM32F407开发板 实验0
//STM32F4工程模板-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

int main(void)
{
	clock_init();
	//gpio_init_pin(GPIOA,GPIO_Pin_6|GPIO_Pin_7,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_UP);

	delay_ms(1000);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	delay_ms(1000);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	return 1;
}

  
