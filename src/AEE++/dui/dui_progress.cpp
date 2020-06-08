#include "dui_progress.h"
#include "dui_stringutil.h"
#include "dui_manager.h"

DUIProgress::DUIProgress()
{
	mClassID = DUIStringUtil::StrHash("progress");
	mPercent = 0;
	mPercentBgFiller = NULL;
	mPercentBdFiller = NULL;
}

DUIProgress::~DUIProgress()
{
	if(mPercentBgFiller)
		mPercentBgFiller->Release();
	mPercentBgFiller = NULL;
	if(mPercentBdFiller)
		mPercentBdFiller->Release();
	mPercentBdFiller = NULL;
}

void DUIProgress::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);

	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_PERCENT_BGFILLER:
		mPercentBgFiller = GetManager()->GetBGFiller(DUIStringUtil::ParseInt(szValue));
		break;

	case DUI_ATTRIBUTE_PERCENT_BDFILLER:
		mPercentBdFiller = GetManager()->GetBorderFiller(DUIStringUtil::ParseInt(szValue));
		break;

	case DUI_ATTRIBUTE_PERCENT:
		mPercent = DUIStringUtil::ParseInt(szValue);
		break;
		
	default:
		DUIControl::SetAttribute(szName,szValue);
		break;
	}
}


void DUIProgress::SetPercent(unsigned int nPercent)
{
	if(nPercent<=100)
		mPercent = nPercent;

}
void DUIProgress::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	TRect rc = GetClientRect();
	rc.x += mLeft + nParentX;
	rc.y += mTop + nParentY;

	TRect rcPercent = rc;
	if(mPercent>100)
		return ;

	rcPercent += GetBorder();
	int progress_width = (rcPercent.width*mPercent)/100;
	if(progress_width == 0)
	{
		return;
	}

	pDC->PushAndSetIntersectClip(rcPercent.x,rcPercent.y,progress_width,rcPercent.height);

	if(mPercentBdFiller)
	{
		mPercentBdFiller->FillRect(pDC, rcPercent);
	}

	if(mPercentBgFiller)
	{
		if(mPercentBdFiller)
			rcPercent -= mPercentBdFiller->Border();
		mPercentBgFiller->FillRect(pDC, rcPercent);
	}
	
	pDC->PopAndRestoreClipRect();
}
