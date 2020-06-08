#include "dui_waitcursor.h"
#include "dui_manager.h"
#include "dui_stringutil.h"
#include "gdi/AEERotateBitmap.h"

#define DUI_PI	3.1415926f
DUIWaitCursor::DUIWaitCursor(bool isAA)
{
	mIsAA = isAA;

	mInterval = 100;
	mCurDegree = 0;
	mTimerId = -1;
	mClassID = DUIStringUtil::StrHash("waitcursor");
	mAlign = ZMAEE_ALIGN_CENTER;
}

DUIWaitCursor::~DUIWaitCursor()
{
	ReleaseBmp();
	if(mTimerId != -1)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimerId);
}

void DUIWaitCursor::ReleaseBmp() {
	if(mBmpHandle.getBmp()) {
		mBmpHandle.releaseBmp();
	}
}
void DUIWaitCursor::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_BITMAP:
		ReleaseBmp();
		mBmpHandle = mManager->LoadBmpHandle(DUIStringUtil::ParseInt(szValue));
		break;
	case DUI_ATTRIBUTE_INTERVAL:
		mInterval = DUIStringUtil::ParseInt(szValue);
		break;
	default:
		DUIControl::SetAttribute(szName, szValue);
		break;
	}
}

void DUIWaitCursor::_OnTimerPaint(int /*nTimerId*/, void* pUser)
{
	DUIWaitCursor* pThis = (DUIWaitCursor*)pUser;
	pThis->OnTimerPaint();
}

void DUIWaitCursor::OnTimerPaint()
{
	mManager->PostRepaintMessage();
}

void DUIWaitCursor::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	int need_for_aa;
	AEE_IBitmap* pBmp = mBmpHandle.getBmp();
	if(pBmp == NULL)
		return;
	
	TRect rc = GetClientRect();
	rc.x += mLeft + nParentX;
	rc.y += mTop + nParentY;
	
	ZMAEE_BitmapInfo bif;
	AEE_IBitmap_GetInfo(pBmp, &bif);
	
	int x = rc.x;
	int y = rc.y;
	switch(mAlign & (ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_RIGHT|ZMAEE_ALIGN_CENTER))
	{
	case ZMAEE_ALIGN_RIGHT:
		x += rc.width - bif.width/2;
		break;
	case ZMAEE_ALIGN_CENTER:
		x += ((rc.width - bif.width/*/2*/)>>1);
		break;
	}
	
	switch(mAlign & (ZMAEE_ALIGN_TOP|ZMAEE_ALIGN_VCENTER|ZMAEE_ALIGN_BOTTOM))
	{
	case ZMAEE_ALIGN_VCENTER:
		y += ((rc.height - bif.height)>>1);
		break;
	case ZMAEE_ALIGN_BOTTOM:
		y += rc.height - bif.height;
		break;
	}

	ZMAEE_LayerInfo lif;
	pDC->GetLayerInfo(pDC->GetActiveLayer(), &lif);

	need_for_aa = (mIsAA && bif.nDepth == ZMAEE_COLORFORMAT_32);
	if (need_for_aa) {
		int i;
		unsigned long* p = (unsigned long*)bif.pBits;
		for (i = bif.width * bif.height - 1; i >= 0; --i) {
			unsigned long t = *p;
			t = (t & 0xf8f8f8ff) >> 3;
			*p++ = t;
		}
	}
	AEERotateBitmap::RotateRender(&lif, DUI_PI * mCurDegree / 180, &bif, x+(bif.width>>1), y + (bif.height>>1), mIsAA);
	if (need_for_aa) {
		int i;
		unsigned long* p = (unsigned long*)bif.pBits;
		for (i = bif.width * bif.height - 1; i >= 0; --i) {
			unsigned long t = *p;
			t = t << 3;
			*p++ = t;
		}
	}


	mCurDegree += 30;
	if(mCurDegree >= 360) mCurDegree %= 360;

	if(mTimerId!=-1) {
		AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimerId);
	}
	mTimerId = AEE_IShell_SetTimer(ZMAEE_GetShell(), 
		mInterval, DUIWaitCursor::_OnTimerPaint, 0, (void*)this);
}
