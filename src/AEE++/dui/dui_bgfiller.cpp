#include "dui_bgfiller.h"
#include "dui_dc.h"

//////////////////////////////////////////////////////////////////////////
//class DUIBGFiller
void DUIBGFiller::AddRef()
{
	++mRef;
}

void DUIBGFiller::Release()
{
	if(--mRef == 0) {
		delete this;
	}
}

//////////////////////////////////////////////////////////////////////////
//class DUIBitmapBGFiller
DUIBitmapBGFiller::DUIBitmapBGFiller(
	DUIBmpHandle bmpHandle, int x, int y, int w, int h, int align, int stretch)
	:mRectBmp(x,y,w,h)
{
	mBitmap = bmpHandle;
	mAlign = align;
	mStretch = stretch;
}

DUIBitmapBGFiller::~DUIBitmapBGFiller()
{
	mBitmap.releaseBmp();
}

void DUIBitmapBGFiller::fillRepeatX(DUIDC* pDC,const TRect& rect,TRect& rcDst)
{
	AEE_IBitmap* pBmp = mBitmap.getLoadBmp();
	if(pBmp == 0)
		return;

	int x = rect.x;
	int y = rect.y;
	if(mAlign&BG_ALIGN_BOTTOM) y = rect.y + rect.height - mRectBmp.height;
	while(x < rect.x + rect.width)
	{
		pDC->DrawBitmap(x, y, pBmp, &mRectBmp, false);
		x += mRectBmp.width;
	}
	rcDst.x  = rect.x;
	rcDst.y = y;
	rcDst.width = rect.width;
	rcDst.height = mRectBmp.height;
}

void DUIBitmapBGFiller::fillRepeatY(DUIDC* pDC,const TRect& rect)
{
	AEE_IBitmap* pBmp = mBitmap.getLoadBmp();
	if(pBmp == 0)
		return;
	int x = rect.x;
	int y = rect.y;
	if(mAlign&BG_ALIGN_RIGHT) x = rect.x + rect.width - mRectBmp.width;

	while(y < rect.y + rect.height)
	{
		pDC->DrawBitmap(x, y, pBmp, &mRectBmp, 0);
		y += mRectBmp.height;
	}
}

void DUIBitmapBGFiller::fillStetch(DUIDC* pDC,const TRect& rect)
{
	AEE_IBitmap* pBmp = mBitmap.getLoadBmp();
	if(pBmp == 0)
		return;
	ZMAEE_BitmapInfo bif;
	AEE_IBitmap_GetInfo(pBmp, &bif);
	pDC->StretchBlt((ZMAEE_Rect*)&rect, &bif, &mRectBmp, 0);
}

void DUIBitmapBGFiller::FillRect(DUIDC* pDC, const TRect& rect)
{
	AEE_IBitmap* pBmp = mBitmap.getLoadBmp();
	if(pBmp == 0)
		return;

	if(mStretch == BG_REPEAT_NO || mStretch == BG_STRETCH_NO)
	{
		pDC->DrawBitmap(rect.x, rect.y , pBmp, &mRectBmp, 0);
	}
	else
	{
		switch(mStretch)
		{
		case BG_REPEAT_X:
			{
				TRect rcDst;
				fillRepeatX(pDC, rect, rcDst);
			}
			break;

		case BG_REPEAT_Y:
			fillRepeatY(pDC, rect);
			break;

		case BG_REPEAT_XY:
			{
				ZMAEE_BitmapInfo bif;
				pDC->GetDCBitmapInfo(&bif);

				TRect rcDst;
				fillRepeatX(pDC, rect, rcDst);
				int y = rect.y + rcDst.height;
				while(y < rect.y + rect.height -1)
				{
					pDC->BitBlt(rect.x, y, &bif, &rcDst, ZMAEE_TRANSFORMAT_NORMAL, 0);
					y += rcDst.height;
				}
			}
			break;
		case BG_STRETCH_X:
			{
				TRect rcDst(rect.x,rect.y,rect.width, mRectBmp.height);
				if(mAlign&BG_ALIGN_BOTTOM) rcDst.y = rect.y + rect.height - mRectBmp.height;
				fillStetch(pDC, rcDst);
			}
			break;
		case BG_STRETCH_Y:
			{
				TRect rcDst(rect.x, rect.y, mRectBmp.width, rect.height);
				if(mAlign&BG_ALIGN_RIGHT) rcDst.x = rect.x + rect.width - mRectBmp.width;
				fillStetch(pDC, rcDst);
			}
			break;
		case BG_STRETCH_XY:
			fillStetch(pDC,  rect);
			break;
			
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//class DUIColorBGFiller
DUIColorBGFiller::DUIColorBGFiller(ZMAEE_Color clr)
{
	mColor = clr;
}

void DUIColorBGFiller::FillRect(DUIDC* pDC, const TRect& rect)
{
	pDC->FillRect(rect.x, rect.y, rect.width, rect.height, mColor);
}

//////////////////////////////////////////////////////////////////////////
//class DUIGradientBGFiller
DUIGradientBGFiller::DUIGradientBGFiller(ZMAEE_GRADIENT_TYPE type, unsigned long* vecColor)
{
	unsigned long nCount = *vecColor;
	int nSize = (nCount*2+1)*sizeof(unsigned long)*2;
	mVecColor = (unsigned long*)AEEAlloc::Alloc(nSize);
	mType = type;
	zmaee_memcpy(mVecColor, vecColor, nSize);
}

DUIGradientBGFiller::~DUIGradientBGFiller()
{
	if(mVecColor){
		AEEAlloc::Free(mVecColor);
		mVecColor = NULL;
	}
}

void DUIGradientBGFiller::_FillGradientV(
	DUIDC* pDC, ZMAEE_Rect* pRect, ZMAEE_Color clrStart, ZMAEE_Color clrEnd)
{
	int  dst_r, dst_g, dst_b, dst_a;
	int  i ,color;
	int  count ,start;
	int  x1, x2, y ;

	x1 = pRect->x ;
	x2 = pRect->x + pRect->width ;
	y  = pRect->y ;

	dst_b = (int)((unsigned char)(clrEnd)) - (int)((unsigned char)(clrStart));
	dst_g = (int)((unsigned char)(clrEnd>>8)) - (int)((unsigned char)(clrStart>>8));
	dst_r = (int)((unsigned char)(clrEnd>>16)) - (int)((unsigned char)(clrStart>>16));
	dst_a = (int)((unsigned char)(clrEnd>>24)) - (int)((unsigned char)(clrStart>>24));

	
	count = pRect->height ;
	start = 0 ;
	for( i = 0 ; i < pRect->height ; i++){
		color = ZMAEE_GET_RGBA(
			(count ? (((dst_r * start) / count) + (clrStart>>16)) : (clrStart>>16)) & 0xFF,
			(count ? (((dst_g * start) / count) + (clrStart>>8)) : (clrStart>>8)) & 0xFF,
			(count ? (((dst_b * start) / count) + (clrStart)) : (clrStart)) & 0xFF,
			(count ? (((dst_a * start) / count) + (clrStart>>24)) : (clrStart>>24)) & 0xFF
			);
		++start ;
		if(x2 - x1 == 1)
			pDC->SetPixel(x1,y,color);
		else
			pDC->DrawLine(x1, y, x2, y,  color);
		y++;
	}
}

void DUIGradientBGFiller::_FillGradientH(
	DUIDC* pDC, ZMAEE_Rect* pRect, ZMAEE_Color clrStart, ZMAEE_Color clrEnd)
{
	int  dst_r, dst_g, dst_b, dst_a;
	int  count ,start;
	int  i , color;
	int  x , y1, y2 ;

	x = pRect->x ;
	y1 = pRect->y ;
	y2 = pRect->y + pRect->height ;

	dst_b = (int)((unsigned char)(clrEnd)) - (int)((unsigned char)(clrStart));
	dst_g = (int)((unsigned char)(clrEnd>>8)) - (int)((unsigned char)(clrStart>>8));
	dst_r = (int)((unsigned char)(clrEnd>>16)) - (int)((unsigned char)(clrStart>>16));
	dst_a = (int)((unsigned char)(clrEnd>>24)) - (int)((unsigned char)(clrStart>>24));

	count = pRect->width ;
	start = 0 ;
	for( i = 0 ; i < pRect->width ; i++){
		color = ZMAEE_GET_RGBA(
			(count ? (((dst_r * start) / count) + (clrStart>>16)) : (clrStart>>16)) & 0xff,
			(count ? (((dst_g * start) / count) + (clrStart>>8)) : (clrStart>>8)) & 0xff,
			(count ? (((dst_b * start) / count) + (clrStart)) : (clrStart)) & 0xff ,
			(count ? (((dst_a * start) / count) + (clrStart>>24)) : (clrStart>>24)) & 0xFF
			);
		++start ;
		if(zmaee_abs(y2-y1) == 1)
			pDC->SetPixel(x,y1,color);
		else
			pDC->DrawLine(x, y1, x, y2,  color);
		x++ ;
	}
}

void DUIGradientBGFiller::_FillGradient(
	DUIDC* pDC, ZMAEE_Rect* pRect, ZMAEE_Color clrStart, ZMAEE_Color clrEnd, ZMAEE_GRADIENT_TYPE type)
{
	ZMAEE_Rect rect;
	if (pDC == NULL || pRect == NULL)
		return;

	rect = *pRect;

	switch(type){
	case ZMAEE_GRADIENT_RECT_VER:
		_FillGradientV(pDC, &rect, clrStart, clrEnd);
		break;
	case ZMAEE_GRADIENT_RECT_HOR:
		_FillGradientH(pDC, &rect, clrStart, clrEnd);
		break;
	}
}

void DUIGradientBGFiller::FillRect(DUIDC* pDC, const TRect& rect)
{
	if(mVecColor == NULL)
		return;

	unsigned long nCount = *mVecColor;
	unsigned long* pColor = mVecColor+1;
	unsigned long idx = 0;

	TRect rc = rect;
#if 0
	while(idx < nCount-1)
	{
		rc.width = (rect.width*100/pColor[1]);
		_FillGradient(pDC, &rc, pColor[0], pColor[2], mType);
		rc.x += rc.width;
		pColor += 2;
		++idx;
	}
#endif

	if(mType == ZMAEE_GRADIENT_RECT_VER)
	{
		while(idx <= nCount-1)
		{
			rc.height = (rect.height*pColor[1]/100);
			_FillGradient(pDC, &rc, pColor[0], pColor[2], mType);
			rc.y += rc.height;
			pColor += 2;
			++idx;
		}
	}
	else if (mType == ZMAEE_GRADIENT_RECT_HOR)
	{
		while(idx <= nCount-1)
		{
			rc.width = (rect.width*pColor[1]/100);
			_FillGradient(pDC, &rc, pColor[0], pColor[2], mType);
			rc.x += rc.width;
			pColor += 2;
			++idx;
		}
	}
	
}
