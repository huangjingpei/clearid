#ifndef _DUI_INDICATOR_H_
#define _DUI_INDICATOR_H_

#include "dui_control.h"

class DUIIndicator : public DUIControl
{
public:
	DUIIndicator();
	virtual ~DUIIndicator();
	virtual void  SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	virtual TSize MeasureSize(int nParentCx, int nParentCy);
	
protected:
	virtual void OnPaintStatusImage(DUIDC* pDC,int nParentX, int nParentY);

private:
	DUIBmpHandle	mBmpHandle;
	int			  mCellInterval;
};

#endif//_DUI_INDICATOR_H_
