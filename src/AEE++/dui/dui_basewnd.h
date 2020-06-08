#ifndef _DUI_BASE_WND_H_
#define _DUI_BASE_WND_H_

#include "dui_window.h"
#include "dui_notifyevent.h"

class DUIManager;

class DUIBaseWnd
{
public:
	DUIBaseWnd(DUIManager* pManager, const char* szWndFile);
	DUIBaseWnd(const char* szWndContent,DUIManager* pManager);
	virtual ~DUIBaseWnd();
	DUIWindow*	 GetInnerWnd(){return mWnd;}
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnResume();
	virtual void OnSuspend();
	virtual void OnNotifyEvent(DUINotifyEvent* pEv);
protected:
	DUIWindow* mWnd;
};

#endif//_DUI_BASE_WND_H_
