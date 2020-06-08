#ifndef _DUI_EDIT_H_
#define _DUI_EDIT_H_

#include "dui_control.h"
#include "AEEDelegate.h"
class DUIEdit : public DUIControl
{
public:
	DUIEdit();
	virtual ~DUIEdit();
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	void OnInputClick();
	friend void InputTextCb(void *pUser, short* buf, int len);
	KString GetText() const {
		return mText;
	}
	void ClearText();
	TFontAttr& GetFontAttr() {return mFontAttr;}
	void  SetOwnerPaint(AEEDelegateBase* pPaintHandler){
		mPaintHandler = pPaintHandler;
	}
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);
protected:

protected:
	ZMAEE_INPUT_TYPE mInputMethod;
	int	    mMaxLen;
	TColor	mTextClr;
	TString mText;
	TString mTextPwd;
	TColor	mTipClr;
	TString mTipInput;
	unsigned short mInputTitle[16];
	TFontAttr mFontAttr;
	AEEDelegateBase* mPaintHandler;
};

#endif//_DUI_EDIT_H_
