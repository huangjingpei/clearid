#include "dui_bdfiller.h"
#include "dui_dc.h"
#ifdef WIN32
#include <stdio.h>
#endif

void DUIBorderFiller::AddRef()
{
	++mRef;
}

void DUIBorderFiller::Release()
{
	if(--mRef == 0) 
	{
		delete this;
	}
}

//////////////////////////////////////////////////////////////////////////
//class DUIColorBorderFiller
DUIColorBorderFiller::DUIColorBorderFiller(ZMAEE_Color clr)
{
	mColor = clr;
}

void DUIColorBorderFiller::FillRect(DUIDC* pDC, const TRect& rect)
{
	const TBorder& bd = Border();
	//top
	if(bd.top > 0)
		pDC->FillRect(rect.x, rect.y, rect.width, bd.top, mColor);
	//left
	if(bd.left > 0)
		pDC->FillRect(rect.x, rect.y, bd.left, rect.height, mColor);
	//right
	if(bd.right > 0)
		pDC->FillRect(rect.x + rect.width - bd.right, rect.y, bd.right, rect.height, mColor);
	//bottom
	if(bd.bottom > 0)
		pDC->FillRect(rect.x, rect.y + rect.height - bd.bottom, rect.width, bd.bottom, mColor);
}

//////////////////////////////////////////////////////////////////////////
//class DUIBitmapBorderFiller
DUIBitmapBorderFiller::DUIBitmapBorderFiller(DUIBmpHandle bmpHandle, int x, int y, int w, int h)
	:mBmpRect(x,y,w,h)
{
	mBitmap = bmpHandle;
}

DUIBitmapBorderFiller::~DUIBitmapBorderFiller()
{
	mBitmap.releaseBmp();
}

void DUIBitmapBorderFiller::FillRect(DUIDC* pDC, const TRect& rect)
{
	AEE_IBitmap* pBmp = mBitmap.getLoadBmp();
	if(pBmp == NULL)
		return;
	
	ZMAEE_BitmapInfo bif = {0};
	AEE_IBitmap_GetInfo(pBmp, &bif);
	int btrans = (bif.clrTrans&0x10000000)?0:1;

	const TBorder& bd = Border();
	//left border
	pDC->StretchBlt(
		rect.x, rect.y + bd.top, bd.left, rect.height - bd.bottom - bd.top, 
		&bif,
		mBmpRect.x, mBmpRect.y + bd.top, bd.left, mBmpRect.height-bd.bottom-bd.top, btrans);

	//top border
	pDC->StretchBlt(
		rect.x + bd.left, rect.y , rect.width - bd.left - bd.right, bd.top,
		&bif,
		mBmpRect.x + bd.left, mBmpRect.y , mBmpRect.width - bd.left - bd.right, bd.top, btrans);

	//right_border
	pDC->StretchBlt(
		rect.x + rect.width - bd.right, rect.y + bd.top, bd.right, rect.height - bd.top - bd.bottom,
		&bif,
		mBmpRect.x + mBmpRect.width - bd.right, mBmpRect.y + bd.top, bd.right, mBmpRect.height - bd.top - bd.bottom, btrans);
	
	//bottom_border
	pDC->StretchBlt(
		rect.x + bd.left, rect.y + rect.height - bd.bottom, rect.width-bd.left-bd.right, bd.bottom,
		&bif,
		mBmpRect.x + bd.left, mBmpRect.y + mBmpRect.height - bd.bottom, mBmpRect.width-bd.left-bd.right, bd.bottom, btrans);

	//top_left
	pDC->BitBlt(rect.x, rect.y, 
		&bif, 
		mBmpRect.x, mBmpRect.y, bd.left, bd.top, 0, btrans);

	//bottom_left
	pDC->BitBlt(rect.x, rect.y + rect.height - bd.bottom, 
		&bif, 
		mBmpRect.x, mBmpRect.y + mBmpRect.height - bd.bottom, bd.left, bd.bottom, 0, btrans);
	
	//right_top
	pDC->BitBlt(rect.x + rect.width - bd.right, rect.y, 
		&bif, 
		mBmpRect.x + mBmpRect.width - bd.right, mBmpRect.y, bd.right,bd.top, 0,btrans);
	
	//right_bottom
	pDC->BitBlt(rect.x + rect.width - bd.right, rect.y + rect.height - bd.bottom, 
		&bif, 
		mBmpRect.x + mBmpRect.width - bd.right, mBmpRect.y + mBmpRect.height - bd.bottom, bd.right, bd.bottom, 0, btrans);
}

//////////////////////////////////////////////////////////////////////////
//class DUIBitmapBorderFiller
DUIBitmapHBorderFiller::DUIBitmapHBorderFiller(DUIBmpHandle bmpHandle, int x, int y, int w, int h)
:mBmpRect(x,y,w,h)
{
	mBitmap = bmpHandle;
}

DUIBitmapHBorderFiller::~DUIBitmapHBorderFiller()
{
	mBitmap.releaseBmp();
}

void DUIBitmapHBorderFiller::FillRect(DUIDC* pDC,const TRect& rect)
{
	AEE_IBitmap* pBmp = mBitmap.getLoadBmp();
	if(pBmp == 0)
		return;

	if(mBorder.left > 0)
	{
		TRect rc(mBmpRect.x, mBmpRect.y, mBorder.left, mBmpRect.height);
		pDC->DrawBitmap(rect.x, rect.y, pBmp, &rc, 1);
	}
	
	if(mBorder.right > 0)
	{
		TRect rc(mBmpRect.x+mBmpRect.width-mBorder.right, mBmpRect.y, mBorder.right, mBmpRect.height);
		pDC->DrawBitmap(rect.x+rect.width-mBorder.right, rect.y, pBmp, &rc, 1);
	}
}

//////////////////////////////////////////////////////////////////////////
//class DUIBitmapVBorderFiller
DUIBitmapVBorderFiller::DUIBitmapVBorderFiller(DUIBmpHandle bmpHandle, int x, int y, int w, int h)
:mBmpRect(x,y,w,h)
{
	mBitmap = bmpHandle;
}

DUIBitmapVBorderFiller::~DUIBitmapVBorderFiller()
{
	mBitmap.releaseBmp();
}

void DUIBitmapVBorderFiller::FillRect(DUIDC* pDC,const TRect& rect)
{
	AEE_IBitmap* pBmp = mBitmap.getLoadBmp();
	if(pBmp == 0)
		return;
	ZMAEE_BitmapInfo bi = {0};
	ZMAEE_Rect rcSrc = {0},desRc = {0};
	if(mBorder.top > 0)
	{
		TRect rc(mBmpRect.x, mBmpRect.y, mBmpRect.width, mBorder.top);
		desRc.x = rect.x;
		desRc.y = rect.y;
		desRc.width = rect.width;
		desRc.height = rc.height;
		if(AEE_IBitmap_GetInfo(pBmp,&bi) == E_ZM_AEE_SUCCESS)
		{	
			rcSrc.x = rc.x;
			rcSrc.y = rc.y;
			rcSrc.width = rc.width;
			rcSrc.height= rc.height;
			pDC->StretchBlt(&desRc, &bi, &rcSrc, 1);
		}
	}
	
	if(mBorder.bottom > 0)
	{
		TRect rc(mBmpRect.x, mBmpRect.y+mBmpRect.height-mBorder.bottom, mBmpRect.width, mBorder.bottom);
		desRc.x = rect.x;
		desRc.y = rect.y + rect.height - mBorder.bottom;
		desRc.width = rect.width;
		desRc.height = rc.height;
		
		if(AEE_IBitmap_GetInfo(pBmp,&bi) == E_ZM_AEE_SUCCESS)
		{
			rcSrc.x = rc.x;
			rcSrc.y = rc.y;
			rcSrc.width = rc.width;
			rcSrc.height= rc.height;
			pDC->StretchBlt(&desRc, &bi, &rcSrc, 1);
		}
	}
}
