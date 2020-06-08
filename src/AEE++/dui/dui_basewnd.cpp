#include "dui_basewnd.h"
#include "dui_manager.h"

DUIBaseWnd::DUIBaseWnd(DUIManager* pManager, const char* szWndFile)
{
	mWnd = (DUIWindow*)pManager->CreateControlByFile(szWndFile);
	if(mWnd) mWnd->SetUserData(this);
}

DUIBaseWnd::DUIBaseWnd(const char* szWndContent,DUIManager* pManager)
{
	mWnd = (DUIWindow*)pManager->CreateControl(szWndContent);
	if(mWnd) mWnd->SetUserData(this);
}

DUIBaseWnd::~DUIBaseWnd()
{
	if(mWnd) delete mWnd;
	mWnd = NULL;
}

void DUIBaseWnd::OnCreate()
{
}

void DUIBaseWnd::OnDestroy()
{
}

void DUIBaseWnd::OnResume()
{
}

void DUIBaseWnd::OnSuspend()
{
}

void DUIBaseWnd::OnNotifyEvent(DUINotifyEvent* /*pEv*/)
{
}
