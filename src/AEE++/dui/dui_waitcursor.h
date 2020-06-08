#ifndef _DUI_WAIT_CURSOR_H_
#define _DUI_WAIT_CURSOR_H_

#include "dui_control.h"

class DUIWaitCursor : public DUIControl
{
public:
	DUIWaitCursor(bool isAA = true);
	virtual ~DUIWaitCursor();
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	
protected:
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);
	static  void _OnTimerPaint(int nTimerId, void* pUser);
	void		 OnTimerPaint();
	void		ReleaseBmp();
private:
	DUIBmpHandle mBmpHandle;
	int			 mInterval;
	int			 mCurDegree;
	int			 mTimerId;
	bool		mIsAA;
};

#endif//_DUI_WAIT_CURSOR_H_
