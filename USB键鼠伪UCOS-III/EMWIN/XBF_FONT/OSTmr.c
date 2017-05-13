#include "OSTmr.h"

OS_TMR Tmr1;
OS_TMR Tmr2;

extern unsigned long lwip_localtime;

void _cbTmr1(void *p_tmr,void *p_arg)
{
	//OSTmrCreate(&Tmr1,"Tmr1",0,1,OS_OPT_TMR_PERIODIC,_cbTmr1,(void *)0,&err);
//OS_CFG_TMR_TASK_RATE_HZ=100hz
}
void _cbTmr2(void *p_tmr,void *p_arg)
{}