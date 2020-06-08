#ifndef _MUI_WINDOW_H_
#define _MUI_WINDOW_H_

#include "zmaee_image.h"
#include "mui_container.h"

class MUIHandler;
class MUIWindow : public MUIContainer
{
public:
	MUIWindow();
	MUIWindow(MUIManager* pMgr, int x, int y, int width, int height, MUIStyle nStyle, MUIHandler* pEvHandler);
	virtual ~MUIWindow();
	virtual void onPaint(MUIDC* pDC);
	virtual bool onPenEv(int ev, int x, int y);
public:
	bool OnEvent(int ev, MUIWidget* pSender, int wparam, int lparam);
	void setCapture(MUIWidget* pWidget);
	MUIWidget* getCapture(){ return mCaptureWidghet; }
	void setHandler(MUIHandler* pHandler) { mEvHandler = pHandler; }
	MUIHandler* getHandler(){return mEvHandler; }
	void SetNeedBg(bool bNeedBg){mNeedBg = bNeedBg;}
protected:
	MUIHandler*		 mEvHandler;
	bool			 mNeedBg;
	MUIWidget*		 mCaptureWidghet;
};

#endif//_MUI_WINDOW_H_
