#ifndef __DUI_BG_FILLER_H__
#define __DUI_BG_FILLER_H__

#include "zmaee_display.h"
#include "AEEBase.h"
#include "dui_util.h"

class DUIDC;

class DUIBGFiller : public AEEBase
{
public:
	enum eBgAlign 
	{
		BG_ALIGN_LEFT		= 1,
		BG_ALIGN_RIGHT		= 2,
		BG_ALIGN_TOP		= 4,
		BG_ALIGN_BOTTOM		= 8
	};
	enum eBgStretchDir
	{
		BG_REPEAT_NO		= 1,
		BG_REPEAT_X			= 2,
		BG_REPEAT_Y			= 4,
		BG_REPEAT_XY		= BG_REPEAT_X|BG_REPEAT_Y,
		
		BG_STRETCH_NO		= 0x10,
		BG_STRETCH_X		= 0x20,
		BG_STRETCH_Y		= 0x40,
		BG_STRETCH_XY		= BG_STRETCH_X|BG_STRETCH_Y
	};
	DUIBGFiller(){
		mRef = 1;
	}
	virtual ~DUIBGFiller(){}
	virtual void FillRect(DUIDC* pDC, const TRect& rect) = 0;
	virtual void UnLoadBmp(){}
	void AddRef();
	void Release();
	void SetID(int nID){
		mID = nID;
	}
	int GetID() const{
		return mID;
	}
	
protected:
	int mRef;
	int mID;
};

class DUIColorBGFiller : public DUIBGFiller
{
public:
	DUIColorBGFiller(ZMAEE_Color clr);
	virtual void FillRect(DUIDC* pDC, const TRect& rect);
	void SetBgColor(TColor clr){ mColor = clr;}
protected:
	TColor mColor;
};

class DUIBitmapBGFiller : public DUIBGFiller
{
public:
	DUIBitmapBGFiller(DUIBmpHandle bmpHandle, int x, int y, int w, int h, int align, int stretch);
	virtual ~DUIBitmapBGFiller();
	virtual void FillRect(DUIDC* pDC,const TRect& rect);
	virtual void UnLoadBmp(){mBitmap.releaseBmp();}
	
protected:
	void fillRepeatX(DUIDC* pDC,const TRect& rect,TRect& rcDst);
	void fillRepeatY(DUIDC* pDC,const TRect& rect);
	void fillStetch(DUIDC* pDC, const TRect& rect);
protected:
	DUIBmpHandle	mBitmap;
	TRect			mRectBmp;
	int				mAlign;
	int				mStretch;
};

class DUIGradientBGFiller : public DUIBGFiller
{
public:
	DUIGradientBGFiller(ZMAEE_GRADIENT_TYPE type, unsigned long* vecColor);
	virtual ~DUIGradientBGFiller();
	virtual void FillRect(DUIDC* pDC, const TRect& rect);
protected:
	void _FillGradientV(DUIDC* pDC, ZMAEE_Rect* pRect, ZMAEE_Color clrStart, ZMAEE_Color clrEnd);
	void _FillGradientH(DUIDC* pDC, ZMAEE_Rect* pRect, ZMAEE_Color clrStart, ZMAEE_Color clrEnd);
	void _FillGradient(DUIDC* pDC, ZMAEE_Rect* pRect, ZMAEE_Color clrStart, ZMAEE_Color clrEnd, ZMAEE_GRADIENT_TYPE type);
protected:
	ZMAEE_GRADIENT_TYPE mType;
	unsigned long* mVecColor;
};

#endif//__DUI_BG_FILLER_H__
