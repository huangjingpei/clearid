#ifndef _DUI_LABEL_H_
#define _DUI_LABEL_H_

#include "dui_control.h"

class DUILabel : public DUIControl
{
public:
	DUILabel();
	virtual ~DUILabel();
	KString GetText() const{return mText;}
	void SetText(KString wcsText);
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	virtual TSize MeasureSize(int nParentCx, int nParentCy);
	TFontAttr& GetFontAttr() {return mFontAttr;}
	
protected:
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);

protected:
	TColor  mClr;
	TString mText;
	TFontAttr mFontAttr;
	int		mIsStrikeThrough;

};

#endif//_DUI_LABEL_H_
