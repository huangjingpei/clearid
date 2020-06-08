
#include "mui_transform.h"
#include "mui_manager.h"
#include "mui_window.h"
#include "mui_scroll.h"

//////////////////////////////////////////////////////////////////////////
//


unsigned long MUITransformBase::getTick()
{
	AEE_IShell* shell = ZMAEE_GetShell();
	return AEE_IShell_GetTickCount(shell);

}

MUITransformBase::~MUITransformBase()
{

}

//////////////////////////////////////////////////////////////////////////
//

MUITransform2D::MUITransform2D(int during)
{
	mOldBitmap = NULL;
	mNewBitmap = NULL;
	mDuring = during;
	mStartX = 0;
	mStartY = 0;
	mEndX = 0;
	mEndY = 0;
	mCurrentX = 0;
	mCurrentY = 0;
	mStartAlpha = 255;
	mEndAlpha = 255;
	mCurrentAlpha = 255;
	mStartScale = 1.0f;
	mEndScale = 1.0f;
	mCurrentScale = 1.0f;
	mTickStart = 0;
	mTickCurrent = 0;
}

MUITransform2D::~MUITransform2D()
{
	if (mOldBitmap) {
		AEE_IBitmap_Release(mOldBitmap);
	}
	if (mNewBitmap) {
		AEE_IBitmap_Release(mNewBitmap);
	}	
}

void MUITransform2D::setDuring(int during)
{
	mDuring = during;
}

void MUITransform2D::setMoveParam(int startX, int startY, int endX, int endY)
{

	mStartX = startX;
	mStartY = startY;
	mEndX = endX;
	mEndY = endY;
	mCurrentX = startX;
	mCurrentY = startY;
}

void MUITransform2D::setAlphaParam(int startAlpha, int endAlpha)
{
	mStartAlpha = startAlpha;
	mEndAlpha = endAlpha;
	mCurrentAlpha = startAlpha;
}

void MUITransform2D::setScaleParam(float startScale, float endScale)
{
	mStartScale = startScale;
	mEndScale = endScale;
	mCurrentScale = startScale;
}

bool MUITransform2D::isFinished()
{
	unsigned long tick = getTick();
	if (tick >= mTickStart + mDuring || mDuring <= 0)
		return true;
	mTickCurrent = tick;	
	int dx = mEndX - mStartX;
	int dy = mEndY - mStartY;
	float percent = (float)(tick - mTickStart) / mDuring;

	float value = MUIScroll::getEaseBackValue(percent);
	

	// Î»ÒÆ
	if (dx < 0) {
		dx = -(int)(value * -dx + 0.5f);
	} else {
		dx = (int)(value * dx + 0.5f);
	}
	
	if (dy < 0) {
		dy = -(int)(value * -dy + 0.5f);
	} else {
		dy = (int)(value * dy + 0.5f);
	}
	
	mCurrentX = mStartX + dx;
	mCurrentY = mStartY + dy;
	
	// alpha
	int alpha = mEndAlpha - mStartAlpha;
	if (alpha < 0) {
		alpha = -(int)(value * -alpha + 0.5f);
	} else {
		alpha = (int)(value * alpha + 0.5f);
	}
	mCurrentAlpha = mStartAlpha + alpha;
	if (mCurrentAlpha < 0) mCurrentAlpha = 0;
	if (mCurrentAlpha > 255) mCurrentAlpha = 255;
	
	// scale
	mCurrentScale = mStartScale + (value * (mEndScale - mStartScale));

	
	return false;
}

void MUITransform2D::setBackWindow(MUIWindow* pWnd, int cf)
{
	if (pWnd == NULL)
		return;
	MUIManager* pManager = pWnd->getManager();
	mOldBitmap = pManager->createWindowBitmap(pWnd, 
		pManager->getWndWidth(), pManager->getWndHeight(), cf);
	
}


void MUITransform2D::setTopWindow(MUIWindow* pWnd, int cf)
{
	if (pWnd == NULL)
		return;
	MUIManager* pManager = pWnd->getManager();
	mNewBitmap = pManager->createWindowBitmap(pWnd, 
		pManager->getWndWidth(), pManager->getWndHeight(), cf);
}

void MUITransform2D::start()
{
	mTickStart = getTick();
}

void MUITransform2D::onDraw(MUIDC* pDC)
{
	ZMAEE_BitmapInfo bi;
	if (mOldBitmap) {
		AEE_IBitmap_GetInfo(mOldBitmap, &bi);
		ZMAEE_Rect rc = {0, 0, bi.width, bi.height};
		pDC->DrawBitmap(0, 0, mOldBitmap, &rc, 0);
	}

	if (mNewBitmap) {
		AEE_IBitmap_GetInfo(mNewBitmap, &bi);
		ZMAEE_Rect rc = {0, 0, bi.width, bi.height};
		if (pDC->IsGL()) {
			float tx = (float)mCurrentX + (float)bi.width / 2;
			float ty = (float)mCurrentY + (float)bi.height / 2;
			bool aa = false;
			if (mCurrentScale != 1.0f) {
				aa = true;
			}

			if (mCurrentScale > 0.001f)
				MUIUtil::DrawBitmapGL(pDC->GetDisplay(), tx, ty, mNewBitmap, &rc, mCurrentScale, mCurrentScale, 0, mCurrentAlpha, aa);
		
		} else {
			if (mCurrentScale != 1.0f) {
				ZMAEE_Rect rcDst;
				rcDst.width = (int)(mCurrentScale * bi.width + 0.5f);
				rcDst.height = (int)(mCurrentScale * bi.height + 0.5f);
				rcDst.x = (mCurrentX * 2 + bi.width - rcDst.width + 1) / 2;
				rcDst.y = (mCurrentY * 2 + bi.height - rcDst.height + 1) / 2;
				if (rcDst.width > 0 && rcDst.height > 0)
					pDC->StretchBlt(&rcDst, &bi, &rc, 0);
			} else {
				pDC->DrawBitmap(mCurrentX, mCurrentY, mNewBitmap, &rc, 0);
			}
			
		}
	}
	
}

//////////////////////////////////////////////////////////////////////////
//

MUITransformLeftIn::MUITransformLeftIn(int during, bool fade) 
	: MUITransform2D(during) 
{
	if (fade) {
		setAlphaParam(0, 255);
	}
}

void MUITransformLeftIn::setTopWindow(MUIWindow* pWnd, int cf)
{
	MUITransform2D::setTopWindow(pWnd, cf);
	if (mNewBitmap) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(mNewBitmap, &bi);
		setMoveParam(-bi.width, 0, 0, 0);
	}
}


//////////////////////////////////////////////////////////////////////////
//

MUITransformLeftOut::MUITransformLeftOut(int during, bool fade) 
	: MUITransform2D(during) 
{
	if (fade) {
		setAlphaParam(255, 0);
	}
}

void MUITransformLeftOut::setTopWindow(MUIWindow* pWnd, int cf)
{
	MUITransform2D::setTopWindow(pWnd, cf);
	if (mNewBitmap) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(mNewBitmap, &bi);
		setMoveParam(0, 0, -bi.width, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
//

MUITransformRightIn::MUITransformRightIn(int during, bool fade) 
	: MUITransform2D(during) 
{
	if (fade) {
		setAlphaParam(0, 255);
	}
}

void MUITransformRightIn::setTopWindow(MUIWindow* pWnd, int cf)
{
	MUITransform2D::setTopWindow(pWnd, cf);
	if (mNewBitmap) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(mNewBitmap, &bi);
		setMoveParam(bi.width, 0, 0, 0);
	}
}


//////////////////////////////////////////////////////////////////////////
//

MUITransformRightOut::MUITransformRightOut(int during, bool fade) 
	: MUITransform2D(during) 
{
	if (fade) {
		setAlphaParam(255, 0);
	}
}

void MUITransformRightOut::setTopWindow(MUIWindow* pWnd, int cf)
{
	MUITransform2D::setTopWindow(pWnd, cf);
	if (mNewBitmap) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(mNewBitmap, &bi);
		setMoveParam(0, 0, bi.width, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
//

MUITransformTopIn::MUITransformTopIn(int during, bool fade) 
	: MUITransform2D(during) 
{
	if (fade) {
		setAlphaParam(0, 255);
	}
}

void MUITransformTopIn::setTopWindow(MUIWindow* pWnd, int cf)
{
	MUITransform2D::setTopWindow(pWnd, cf);
	if (mNewBitmap) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(mNewBitmap, &bi);
		setMoveParam(0, -bi.height, 0, 0);
	}
}


//////////////////////////////////////////////////////////////////////////
//

MUITransformTopOut::MUITransformTopOut(int during, bool fade) 
	: MUITransform2D(during) 
{
	if (fade) {
		setAlphaParam(255, 0);
	}
}

void MUITransformTopOut::setTopWindow(MUIWindow* pWnd, int cf)
{
	MUITransform2D::setTopWindow(pWnd, cf);
	if (mNewBitmap) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(mNewBitmap, &bi);
		setMoveParam(0, 0, 0, -bi.height);
	}
}

//////////////////////////////////////////////////////////////////////////
//

MUITransformBottomIn::MUITransformBottomIn(int during, bool fade) 
	: MUITransform2D(during) 
{
	if (fade) {
		setAlphaParam(0, 255);
	}
}

void MUITransformBottomIn::setTopWindow(MUIWindow* pWnd, int cf)
{
	MUITransform2D::setTopWindow(pWnd, cf);
	if (mNewBitmap) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(mNewBitmap, &bi);
		setMoveParam(0, bi.height, 0, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
//

MUITransformBottomOut::MUITransformBottomOut(int during, bool fade) 
	: MUITransform2D(during) 
{
	if (fade) {
		setAlphaParam(255, 0);
	}
}

void MUITransformBottomOut::setTopWindow(MUIWindow* pWnd, int cf)
{
	MUITransform2D::setTopWindow(pWnd, cf);
	if (mNewBitmap) {
		ZMAEE_BitmapInfo bi;
		AEE_IBitmap_GetInfo(mNewBitmap, &bi);
		setMoveParam(0, 0, 0, bi.height);
	}
}

//////////////////////////////////////////////////////////////////////////
//

MUITransformZoomIn::MUITransformZoomIn(int during, bool fade)
	: MUITransform2D(during)
{
	setScaleParam(0.0f, 1.0f);
	if (fade) {
		setAlphaParam(0, 255);
	}	
}

//////////////////////////////////////////////////////////////////////////
//

MUITransformZoomOut::MUITransformZoomOut(int during, bool fade)
	: MUITransform2D(during)
{
	setScaleParam(1.0f, 0.0f);
	if (fade) {	
		setAlphaParam(255, 0);
	}		
}


//////////////////////////////////////////////////////////////////////////
//

MUITransformFadeIn::MUITransformFadeIn(int during)
	: MUITransform2D(during)
{
	setAlphaParam(0, 255);
}


//////////////////////////////////////////////////////////////////////////
//

MUITransformFadeOut::MUITransformFadeOut(int during)
	: MUITransform2D(during)
{
	setAlphaParam(255, 0);
}
