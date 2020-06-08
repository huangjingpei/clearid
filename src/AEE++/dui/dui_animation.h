#ifndef _DUI_ANIMATION_H_
#define _DUI_ANIMATION_H_

#include "dui_control.h"

class DUIAnimation : public DUIControl
{
public:
	DUIAnimation();
	virtual ~DUIAnimation();
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	
protected:
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);
	static  void _OnTimerPaint(int nTimerId, void* pUser);
	void OnTimerPaint();
protected:
	DUIBmpHandle mBmpHandle;
	int			 mFrameWidth;
	int			 mInterval;
	int			 mIdx;
	int			 mTimerId;
};

#endif//_DUI_ANIMATION_H_
