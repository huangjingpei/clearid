#include "dui_indicator.h"
#include "dui_manager.h"
#include "dui_pages.h"
#include "dui_stringutil.h"


DUIIndicator::DUIIndicator()
{
	mCellInterval = 5;
	mClassID = DUIStringUtil::StrHash("indicator");
}

DUIIndicator::~DUIIndicator()
{
	mBmpHandle.releaseBmp();
}

void  DUIIndicator::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_BITMAP:
		mBmpHandle.releaseBmp();		
		mBmpHandle = mManager->LoadBmpHandle(DUIStringUtil::ParseInt(szValue));
		break;
	default:
		DUIControl::SetAttribute(szName, szValue);
		break;
	}
}

TSize DUIIndicator::MeasureSize(int /*nParentCx*/, int /*nParentCy*/)
{
	ZMAEE_BitmapInfo bif={0};
	DUIPages* pPages = (DUIPages*)mParent;
	if(AEE_IBitmap* pBmp = mBmpHandle.getBmp()) AEE_IBitmap_GetInfo(pBmp, &bif);
	return TSize((bif.width/2 + mCellInterval) * pPages->GetPageCount(), bif.height);
}

void DUIIndicator::OnPaintStatusImage(DUIDC* pDC,int nParentX, int nParentY)
{
	AEE_IBitmap* pBmp = mBmpHandle.getBmp();
	if(pBmp == NULL)
		return;

	ZMAEE_BitmapInfo bif={0};
	DUIPages* pPages = (DUIPages*)mParent;
	if(pBmp) AEE_IBitmap_GetInfo(pBmp, &bif);

	TRect rect = GetClientRect();
	rect.x += mLeft + nParentX;
	rect.y += mTop + nParentY;
	
	if(pPages->GetPageCount() <= 1)
		return;

	int nCurPage = pPages->GetCurIdx();
	for(int i = 0; i < pPages->GetPageCount(); ++i)
	{
		if(i == nCurPage)
		{
			TRect rc(0,0,bif.width/2, bif.height);
			pDC->DrawBitmap(rect.x, rect.y, pBmp, &rc, 1);
		}
		else
		{
			TRect rc(bif.width/2, 0, bif.width/2, bif.height);
			pDC->DrawBitmap(rect.x, rect.y, pBmp, &rc, 1);
		}
		rect.x += bif.width/2 + mCellInterval;
	}
}
