#ifndef __DUI_BUTTON_H__
#define __DUI_BUTTON_H__

#include "dui_control.h"
#include "AEEDelegate.h"

class DUIButton : public DUIControl
{
public:
	DUIButton();
	virtual ~DUIButton();
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	KString GetText() {return mText;}
	void SetText(TString wcsText){
		if(wcsText)
		{
			if(mText) AEEAlloc::Free(mText);
			mText = (TString)AEEAlloc::Alloc(2*(zmaee_wcslen(wcsText)+1));
			zmaee_wcscpy(mText, wcsText);
		}
	};
	TFontAttr& GetFontAttr() {return mFontAttr;}
	AEE_IImage* GetImage(){return mImage;}
	AEE_IBitmap* GetBmp(){return mBmpHandle.getBmp();}

	void   SetPushedImage(AEE_IImage* pImage){
		if(mImagePushed)
			AEE_IImage_Release(mImagePushed);
		mImagePushed = pImage;
	}

	// by zm_zhd
	void SetPushBmp(AEE_IBitmap* pBmp){
		if(mBitmapPushed){
			AEE_IBitmap_Release(mBitmapPushed);
		}
		mBitmapPushed = pBmp;
	}
	
protected:
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);

private:
	AEE_IImage*			mImage;
	DUIBmpHandle		mBmpHandle;	
	TString				mText;
	TColor				mColor;
	TFontAttr			mFontAttr;

	AEE_IImage*			mImagePushed;

	AEE_IBitmap*		mBitmapPushed;	//by zm_zhd
};

#endif//__DUI_BUTTON_H__
