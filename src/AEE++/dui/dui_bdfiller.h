#ifndef __DUI_BORDER_FILLER_H__
#define __DUI_BORDER_FILLER_H__

#include "zmaee_display.h"
#include "AEEBase.h"
#include "dui_util.h"

class DUIDC;

class DUIBorderFiller
{
protected:
	virtual ~DUIBorderFiller(){
	}
public:
	DUIBorderFiller(){
		mRef=1;
	}
	void AddRef();
	void Release();
	void SetID(int nID){
		mID = nID;
	}
	int GetID() const{
		return mID;
	}
	const TBorder& Border() const{ 
		return mBorder; 
	}
	void SetBorder(int l,int  t, int r, int b){
		ZMAEE_LTRB_ASSIGN(mBorder,l,r,t,b);
	}
	virtual void FillRect(DUIDC* pDC,const TRect& rect) = 0;
	virtual void UnLoadBmp(){}
protected:
	int		mRef;
	int		mID;
	TBorder mBorder;
};

class DUIColorBorderFiller : public DUIBorderFiller
{
public:
	DUIColorBorderFiller(ZMAEE_Color clr);
	virtual void FillRect(DUIDC* pDC, const TRect& rect);
protected:
	ZMAEE_Color mColor;
};

class DUIBitmapBorderFiller : public DUIBorderFiller
{
public:
	DUIBitmapBorderFiller(DUIBmpHandle bmpHandle, int x, int y, int w, int h);
	virtual ~DUIBitmapBorderFiller();
	virtual void FillRect(DUIDC* pDC,const TRect& rect);
	virtual void UnLoadBmp(){mBitmap.releaseBmp();}
protected:
	DUIBmpHandle mBitmap;
	TRect		 mBmpRect;
};

class DUIBitmapHBorderFiller : public DUIBorderFiller
{
public:
	DUIBitmapHBorderFiller(DUIBmpHandle bmpHandle, int x, int y, int w, int h);
	virtual ~DUIBitmapHBorderFiller();
	virtual void FillRect(DUIDC* pDC,const TRect& rect);
	virtual void UnLoadBmp(){mBitmap.releaseBmp();}
protected:
	DUIBmpHandle mBitmap;
	TRect		 mBmpRect;
};

class DUIBitmapVBorderFiller : public DUIBorderFiller
{
public:
	DUIBitmapVBorderFiller(DUIBmpHandle bmpHandle, int x, int y, int w, int h);
	virtual ~DUIBitmapVBorderFiller();
	virtual void FillRect(DUIDC* pDC,const TRect& rect);
	virtual void UnLoadBmp(){mBitmap.releaseBmp();}
protected:
	DUIBmpHandle mBitmap;
	TRect		 mBmpRect;
};

#endif//__DUI_BORDER_FILLER_H__
