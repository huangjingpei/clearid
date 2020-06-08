#ifndef _DUI_NOTIFY_EVENT_H_
#define _DUI_NOTIFY_EVENT_H_

#include "dui_control.h"

enum _DUINotifyType
{
	DUI_NOTIFY_CLICK = 1,
	DUI_NOTIFY_MOTION,
	DUI_NOTIFY_KEYBORD,
	DUI_NOTIFY_VALUE_CHANGE,
	DUI_NOTIFY_LONG_TOUCH,
	DUI_NOTIFY_DESTROY,

	DUI_NOTIFY_LIST_ITEMCLICK,	//wParam = tickcount(up) - tickcount(down), lParam = (DUIListItem*)
	DUI_NOTIFY_LIST_GETPREPAGE, //wParam = pageIdx, lParam = pageSize
	DUI_NOTIFY_LIST_GETNEXTPAGE //wParam = pageIdx, lParam = pageSize
	//add by lts at 2015-10-24
	,DUI_NOTIFY_PEN_DOWN,
	DUI_NOTIFY_PEN_MOVE,
	DUI_NOTIFY_PEN_UP,
	DUI_NOTIFY_SLIDE,
	DUI_NOTIFY_TYPE_MAX
};

typedef int DUINotifyType;

struct DUINotifyEvent
{
	DUINotifyType	mType;
	DUIControl*		mOwnerWnd;
	DUIControl*		mSender;
	unsigned long	wParam;
	unsigned long	lParam;
};

#endif//_DUI_NOTIFY_EVENT_H_
