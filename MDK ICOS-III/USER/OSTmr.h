#ifndef OSTMR_H
#define OSTMR_H

#include "init.h"


extern OS_TMR Tmr1;
extern OS_TMR Tmr2;

void _cbTmr1(void *p_tmr,void *p_arg);
void _cbTmr2(void *p_tmr,void *p_arg);

#endif