
#include "demo.h"
#include "graphy.h"



 char string[300];
 char hanzi[]="123456";

void MainTask(void)
{
	GUI_Init();
	GUI_SetBkColor(GUI_GREEN);
	GUI_Clear();
	GUI_SetBkColor(GUI_BLUE);
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

	create_graphy();

	while(1)
	{
		GUI_Delay(10);	
	}
}
