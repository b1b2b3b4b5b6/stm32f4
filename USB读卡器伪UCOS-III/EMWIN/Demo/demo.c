#include "demo.h"
#include "graphy.h"
#include "key.h"
void MainTask(void)
{
	OS_ERR err;
	CPU_TS ts;
	void  *f;
	CPU_INT16U size;
	GUI_Init();
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);
	WM_SetCreateFlags(WM_CF_MEMDEV);

	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX); 
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	MENU_SetDefaultSkin(MENU_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);
	GUI_SetFont(&GUI_Font8x16);
	GUI_UC_SetEncodeUTF8();
	GUI_DispString("UsualFontOK!\n");

	Create_XBF16("0:/EMWIN_FONT/16.xbf");
	Create_XBF24("0:/EMWIN_FONT/24.xbf");
	GUI_SetFont(&XBF16_Font);
	GUI_DispString("XBF16-OK!\n");
	GUI_SetFont(&XBF24_Font);
	GUI_DispString("XBF24-OK!\n");	
	GUI_SetFont(&XBF16_Font);

	while(1)
	{
		f=OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&size,&ts,&err);
		
		if(GUI_GetDispPosY()>200) 
		{
			GUI_GotoXY(0,0);
			GUI_Clear();
		}
		GUI_DispString((char *)f);
		GUI_Exec();
	}
}


