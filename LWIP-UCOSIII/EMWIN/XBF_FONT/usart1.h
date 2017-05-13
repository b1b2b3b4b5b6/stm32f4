#ifndef USART1_H
#define USART1_H

#include "init.h"

void usart1_init(u32 bound);
void usart1_send_byte(u8 data);
void usart1_send_buff(u8 *p,u32 n);
#endif