#ifndef _DUI_PROGRESS_H_
#define _DUI_PROGRESS_H_

#include "dui_control.h"

class DUIProgress : public DUIControl
{
public:
	DUIProgress();
	virtual ~DUIProgress();
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);

	void SetPercent(unsigned int nPercent);
	unsigned int GetPercent(){return mPercent;}

private:
	unsigned int	mPercent;
	DUIBGFiller*	 mPercentBgFiller;
	DUIBorderFiller* mPercentBdFiller;
};

#endif//_DUI_PROGRESS_H_
