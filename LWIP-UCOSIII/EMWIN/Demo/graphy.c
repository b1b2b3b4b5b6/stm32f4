#include "graphy.h"
#include "adc_sample.h"


/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GRAPH_DATA_Handle  _ahData; // Array of handles for the GRAPH_DATA objects
static GRAPH_SCALE_Handle _hScaleV;   // Handle of vertical scale
static GRAPH_SCALE_Handle _hScaleH;   // Handle of horizontal scale

#define ID_FRAMEWIN_0    (GUI_ID_USER + 0x00)
#define ID_GRAPH_0    (GUI_ID_USER + 0x01)
#define ID_SLIDER_0    (GUI_ID_USER + 0x02)
#define ID_SLIDER_1    (GUI_ID_USER + 0x03)
#define ID_BUTTON_0    (GUI_ID_USER + 0x04)
#define ID_TEXT_0    (GUI_ID_USER + 0x05)
#define ID_TEXT_1    (GUI_ID_USER + 0x06)
#define ID_TEXT_2    (GUI_ID_USER + 0x0D)
#define ID_TEXT_3    (GUI_ID_USER + 0x0E)
#define ID_TEXT_4    (GUI_ID_USER + 0x0F)
#define ID_TEXT_5    (GUI_ID_USER + 0x10)
#define ID_TEXT_6    (GUI_ID_USER + 0x11)
#define ID_TEXT_7    (GUI_ID_USER + 0x12)
#define ID_TEXT_8    (GUI_ID_USER + 0x13)
#define ID_TEXT_9    (GUI_ID_USER + 0x14)
//
// Dialog ressource
//

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 320, 240, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 0, 312, 153},
  { SLIDER_CreateIndirect, "SliderX", ID_SLIDER_0, 20, 155, 80, 20, 0, 0x0, 0 },
  { SLIDER_CreateIndirect, "SliderY", ID_SLIDER_1, 20, 181, 80, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "AUTO", ID_BUTTON_0, 30, 204, 62, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "X", ID_TEXT_0, 9, 155, 6, 12, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Y", ID_TEXT_1, 9, 181, 6, 12, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "f:", ID_TEXT_2, 107, 153, 24, 12, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "T:", ID_TEXT_3, 107, 172, 24, 12, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "max:", ID_TEXT_4, 107, 189, 24, 12, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "min:", ID_TEXT_5, 107, 206, 23, 12, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_6, 134, 153, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_7, 134, 172, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_8, 134, 189, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_9, 134, 206, 80, 20, 0, 0x0, 0 },
};



static void _AddValues(void) {
	unsigned i;
	  for (i =0; i<320;i++)
	  {
		  GRAPH_DATA_YT_AddValue(_ahData, ADC1OscConver[i]/30);
	  }
}

static void _UserDraw(WM_HWIN hWin, int Stage)
{
  if (Stage == GRAPH_DRAW_LAST)
  {
		char acText[] = "V";
		GUI_RECT Rect;
		GUI_RECT RectInvalid;
		int FontSizeY;

		GUI_SetFont(&GUI_Font13_ASCII);
		FontSizeY = GUI_GetFontSizeY();
		WM_GetInsideRect(&Rect);
		WM_GetInvalidRect(hWin, &RectInvalid);
		Rect.x1 = Rect.x0 + FontSizeY;
		GUI_SetColor(GUI_YELLOW);
		GUI_DispStringInRectEx(acText, &Rect, GUI_TA_HCENTER, strlen(acText), GUI_ROTATE_CCW);
  }
}


static void _cbCallback(WM_MESSAGE * pMsg) {
	int      NCode;
	int      Id;
	int      Value;
	WM_HWIN  hDlg;
	WM_HWIN  hItem;
	hDlg = pMsg->hWin;
	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
			hItem = WM_GetDialogItem(hDlg, ID_GRAPH_0);
			// Add graphs
			_ahData = GRAPH_DATA_YT_Create(GUI_RED, 320, 0, 0);
			GRAPH_AttachData(hItem, _ahData);
			// Set graph attributes
			GRAPH_SetGridVis(hItem, 1);
			GRAPH_SetUserDraw(hItem, _UserDraw);

			_hScaleV = GRAPH_SCALE_Create( 35, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 10);
			GRAPH_SCALE_SetTextColor(_hScaleV, GUI_YELLOW);
			GRAPH_AttachScale(hItem, _hScaleV);

			_hScaleH = GRAPH_SCALE_Create(140, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 5);
			GRAPH_SCALE_SetTextColor(_hScaleH, GUI_DARKGREEN);
			GRAPH_AttachScale(hItem, _hScaleH);

			hItem = WM_GetDialogItem(hDlg, ID_SLIDER_0);
			SLIDER_SetRange(hItem, 0, 10);
			SLIDER_SetValue(hItem, 5);
			SLIDER_SetNumTicks(hItem, 6);

			hItem = WM_GetDialogItem(hDlg, ID_SLIDER_1);
			SLIDER_SetRange(hItem, 0, 20);
			SLIDER_SetValue(hItem, 5);
			SLIDER_SetNumTicks(hItem, 6);
		break;
		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc);      // Id of widget
			NCode = pMsg->Data.v;                 // Notification code
			switch (NCode)
			{
				case WM_NOTIFICATION_VALUE_CHANGED:
				switch(Id)
				{
					case ID_SLIDER_0:
						//
						// Set horizontal grid spacing
						//
						hItem = WM_GetDialogItem(hDlg, ID_GRAPH_0);
						Value = SLIDER_GetValue(pMsg->hWinSrc) * 10;
						GRAPH_SetGridDistX(hItem, Value);
						GRAPH_SCALE_SetTickDist(_hScaleH, Value);
					break;
					case ID_SLIDER_1:
						//
						// Set vertical grid spacing
						//
						hItem = WM_GetDialogItem(hDlg, ID_GRAPH_0);
						Value = SLIDER_GetValue(pMsg->hWinSrc) * 5;
						GRAPH_SetGridDistY(hItem, Value);
						GRAPH_SCALE_SetTickDist(_hScaleV, Value);
					break;
				}
			}
		break;
		default:
		WM_DefaultProc(pMsg);
	}
}


void create_graphy(void)
{
  OS_ERR err;
  CPU_TS ts;
  WM_HWIN hWin;
  WM_HWIN hItem;
  volatile float t,f;
  char s[100];
  hWin=GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbCallback, 0, 0, 0);
  while(1)
  {
	TIM_Cmd(TIM4,ENABLE);
	OSTaskSemPend(	0,
					OS_OPT_PEND_BLOCKING,
					&ts,
					&err	);
	
	StartSample();
	
	OSTaskSemPend(	0,
					OS_OPT_PEND_BLOCKING,
					&ts,
					&err	);
  	_AddValues();
	
	f=(float)frequency;
	hItem = WM_GetDialogItem(hWin, ID_TEXT_6);
	if(f>1000)	sprintf(s,"%.3fkHz",f/1000);
	else sprintf(s,"%.0fHz",f);
	TEXT_SetText(hItem,s);
	
	hItem = WM_GetDialogItem(hWin, ID_TEXT_7);
	t=1/f;
	if(t<0.001) sprintf(s,"%.3fus",t*1000000);
	else sprintf(s,"%.3fms",t*1000);
	TEXT_SetText(hItem,s);
	
  	GUI_Exec();
	
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
  }
}
