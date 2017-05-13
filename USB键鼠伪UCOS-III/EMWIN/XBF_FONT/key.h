#ifndef KEY_H
#define KEY_H

#include "init.h"

extern OS_SEM KeySem_0;
extern OS_SEM KeySem_1;

#define	KEY_0		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)
#define	KEY_1		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)

void key_exit_init();
void key_init();
void key_scan(void);

#endif