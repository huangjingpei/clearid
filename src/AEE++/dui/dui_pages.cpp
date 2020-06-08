#include "dui_pages.h"
#include "dui_stringutil.h"
#include "layout/dui_layout_tab.h"
#include "dui_manager.h"

DUIPages::DUIPages()
{
	mClassID = DUIStringUtil::StrHash("pages");
	mInterval = 3*1000;
	mTimerId = -1;
	mAutoScroll = false;
}

DUIPages::~DUIPages()
{
	if(mTimerId != -1)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimerId);
}

void DUIPages::SetRect(int x, int y, int cx, int cy)
{
	if(mLayout == NULL) 
		mLayout = new DUILayoutTab();
	DUIContainer::SetRect(x, y, cx, cy);
}

int	DUIPages::GetPageCount()
{
	int nCount = 0;
	for(DUIControls::iterator it = mItems.begin(); it != mItems.end(); ++it)
	{
		if((*it)->GetRelativePos() == NULL)
			++nCount;
	}

	return nCount;
}

int	DUIPages::GetCurIdx()
{
	return (mScrollX/GetClientRect().width);
}

void DUIPages::SetCurIdx(int idx)
{
	mScrollX = (idx * GetClientRect().width);
	mManager->PostRepaintMessage();

}


void DUIPages::_OnTimerScroll(int nTimerId, void* pUser)
{

	DUIPages* pThis = (DUIPages*)pUser;
	pThis->OnTimerScroll();
}
void DUIPages::OnTimerScroll()
{
	int curIdx = GetCurIdx();
	curIdx++;
	if(curIdx>=GetPageCount())
		curIdx = 0;
	SetCurIdx(curIdx);
}


void DUIPages::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	DUIContainer::OnPaintContent(pDC,nParentX,nParentY);
	if(mAutoScroll)
	{
		if(mTimerId!=-1) 
		{
			AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimerId);
		}
		mTimerId = AEE_IShell_SetTimer(ZMAEE_GetShell(), 
			mInterval, DUIPages::_OnTimerScroll, 0, (void*)this);
	}
}

void DUIPages::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_PAGE_AUTOSCROLL:
		mAutoScroll = DUIStringUtil::ParseBool(szValue);
		break;
		
	case DUI_ATTRIBUTE_INTERVAL:
		mInterval = DUIStringUtil::ParseInt(szValue);
		break;
	default:
		DUIContainer::SetAttribute(szName, szValue);
		break;
	}
}