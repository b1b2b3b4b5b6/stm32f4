#include "OSTmr.h"

OS_TMR Tmr1;
OS_TMR Tmr2;

extern unsigned long lwip_localtime;

void _cbTmr1(void *p_tmr,void *p_arg)
{
	lwip_localtime +=10; 
}
void _cbTmr2(void *p_tmr,void *p_arg)
{}