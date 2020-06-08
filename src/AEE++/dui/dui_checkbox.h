#ifndef _DUI_CHECKBOX_H_
#define _DUI_CHECKBOX_H_

#include "zmaee_bitmap.h"
#include "dui_control.h"
struct  DUIDrawContext
{
	DUIControl* mControl;
	TRect*		mRect;
	void*		mUser;
};
class DUICheckBox : public DUIControl
{
public:
	DUICheckBox();
	virtual ~DUICheckBox();
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	virtual void SetOwnerPaint(void* pHandler) ;
	
protected:
	virtual void OnPaintStatusImage(DUIDC* pDC, int nParentX, int nParentY);
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);
private:
	DUIBmpHandle	mBmpHandle;	
	TString		 mText;
	TColor		 mTextClr;
	ZMAEE_TextAlign mTextAlign;
	void *		mOwnerPaintHandler;

};

#endif//_DUI_CHECKBOX_H_
