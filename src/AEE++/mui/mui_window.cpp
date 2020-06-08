#include "mui_window.h"
#include "mui_dc.h"
#include "mui_wndhandler.h"

MUIWindow::MUIWindow()
{
	mEvHandler = NULL;
	mNeedBg = true;
	mCaptureWidghet = NULL;
}

MUIWindow::MUIWindow(MUIManager* pMgr, int x, int y, int width, int height, 
					 MUIStyle nStyle, MUIHandler* pEvHandler)
:MUIContainer(x, y, width, height, nStyle)
{
	mMgr = pMgr;
	mEvHandler = pEvHandler;
	mNeedBg = true;
	mCaptureWidghet = NULL;
}

MUIWindow::~MUIWindow()
{
	if(mEvHandler) delete mEvHandler;
}

bool MUIWindow::onPenEv(int ev, int x, int y)
{
	//if(mCaptureWidghet!= NULL && mCaptureWidghet->onPenEv(ev, x, y))
	//	return true;
	return MUIContainer::onPenEv(ev, x, y);
}

void MUIWindow::onPaint(MUIDC* pDC)
{
	MUIRect rc = getRect();
	if(mNeedBg) mMgr->drawBackground(rc.x, rc.y, rc.width, rc.height);
	MUIContainer::onPaint(pDC);
}


bool MUIWindow::OnEvent(int ev, MUIWidget* pSender, int wparam, int lparam)
{
	if(mEvHandler)
	{
		MUIWndEvent wndEv;
		wndEv.owner = this;
		wndEv.sender = pSender;
		wndEv.ev = ev;
		wndEv.wparam = wparam;
		wndEv.lparam = lparam;
		return (mEvHandler->onHandleEv(&wndEv) != 0);
	}
	return false;
}

void MUIWindow::setCapture(MUIWidget* pWidget)
{
	if(mCaptureWidghet) {
		int nStyle = mCaptureWidghet->getStyle();
		nStyle &= ~MUI_STYLE_PUSHED;
		mCaptureWidghet->setStyle(nStyle);
		MUIRect rc = mCaptureWidghet->getRect();
		mMgr->redraw(&rc);
	}
	mCaptureWidghet = pWidget;
}
