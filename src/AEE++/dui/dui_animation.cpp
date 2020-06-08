#include "dui_animation.h"
#include "dui_stringutil.h"
#include "dui_manager.h"

DUIAnimation::DUIAnimation()
{
	mInterval = 250;
	mIdx = 0;
	mFrameWidth = 32;
	mTimerId = -1;
	mClassID = DUIStringUtil::StrHash("animation");
}

DUIAnimation::~DUIAnimation()
{
	mBmpHandle.releaseBmp();
	
	if(mTimerId != -1)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimerId);
}

void DUIAnimation::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_BITMAP:
		mBmpHandle.releaseBmp();
		mBmpHandle = mManager->LoadBmpHandle(DUIStringUtil::ParseInt(szValue));
		break;
	case DUI_ATTRIBUTE_INTERVAL:
		mInterval = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_FRAMEWIDTH:
		mFrameWidth = DUIStringUtil::ParseInt(szValue);
		break;
	default:
		DUIControl::SetAttribute(szName, szValue);
		break;
	}
}

void DUIAnimation::_OnTimerPaint(int /*nTimerId*/, void* pUser)
{
	DUIAnimation* pThis = (DUIAnimation*)pUser;
	pThis->OnTimerPaint();
}

void DUIAnimation::OnTimerPaint()
{
	mManager->PostRepaintMessage();
}

void DUIAnimation::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	AEE_IBitmap* pBmp = mBmpHandle.getLoadBmp();
	if(pBmp == NULL)
		return;

	TRect rc = GetClientRect();
	rc.x += mLeft + nParentX;
	rc.y += mTop + nParentY;

	ZMAEE_BitmapInfo bif;
	AEE_IBitmap_GetInfo(pBmp, &bif);
	TRect rcBmp = TRect(mIdx*mFrameWidth, 0, mFrameWidth, bif.height);
	
	int x = rc.x;
	int y = rc.y;
	switch(mAlign & (ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_RIGHT|ZMAEE_ALIGN_CENTER))
	{
	case ZMAEE_ALIGN_RIGHT:
		x += rc.width - bif.width/2;
		break;
	case ZMAEE_ALIGN_CENTER:
		x += ((rc.width-bif.width/2)>>1);
		break;
	}

	switch(mAlign & (ZMAEE_ALIGN_TOP|ZMAEE_ALIGN_VCENTER|ZMAEE_ALIGN_BOTTOM))
	{
	case ZMAEE_ALIGN_VCENTER:
		y += ((rc.height-bif.height)>>1);
		break;
	case ZMAEE_ALIGN_BOTTOM:
		y += rc.height-bif.height;
		break;
	}

	pDC->DrawBitmap(x, y, pBmp, &rcBmp, 1);
	mIdx++;
	if(mIdx >= bif.width/mFrameWidth)
		mIdx = 0;
	
	if(mTimerId!=-1)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimerId);
	AEE_IShell_SetTimer(ZMAEE_GetShell(), mInterval, DUIAnimation::_OnTimerPaint, 0, (void*)this);
}
