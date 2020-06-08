#include "dui_listcontrol.h"
#include "dui_stringutil.h"
#include "dui_manager.h"
#include "zmaee_helper.h"
#include "zmaee_shell.h"
DUIListControl::DUIListControl()
{
	mClassID = DUIStringUtil::StrHash("listcontainer");
	mPageTotal = 0;
	mPageSize = 0;
	mPageIndex = 0;
	mSelectIndex = -1;
	mPenDownIndex = -1;
	mScrollY = 0;
	mUsePager = 0;
	mContentHeight = 0;
	mStyle |= UISTYLE_CANFOCUS;
	mLongTouchIvl = 1000;
	mTimerID = -1;
	mMaxDataItem = 0;
}


DUIListControl::~DUIListControl()
{

	if(mTimerID != -1)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(),mTimerID);
	mTimerID = -1;

	for(AEEList<DUIListItem*>::iterator it = mDataItems.begin(); it != mDataItems.end(); ++it)
		delete (*it);

}

void DUIListControl::SetRect(int x, int y, int cx, int cy)
{
	DUIControl::SetRect(x, y, cx, cy);

	AEEList<DUIListItem*>::iterator it;
	mContentHeight = 0;
	for(it = mDataItems.begin();it!= mDataItems.end();++it)
	{
		mContentHeight += (*it)->GetItemHeight();
		
	}
	
}

TSize DUIListControl::MeasureSize(int nParentCx, int nParentCy)
{
	TSize size = DUIControl::MeasureSize(nParentCx, nParentCy);
	return size;
}

void DUIListControl::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);

	if(nNameHash == DUIStringUtil::StrHash("usepager"))
		mUsePager = DUIStringUtil::ParseBool(szValue);

	else DUIControl::SetAttribute(szName, szValue);
}

void DUIListControl::MakeSelectItemVisible()
{
	TRect rc = GetWindowRect();
	TRect rcItem = rc;

	int top = 0;//rc.y;
	int i = 0;
	AEEList<DUIListItem*>::iterator it;
	for(it = mDataItems.begin(); it != mDataItems.end(); ++it)
	{
		if(i == mSelectIndex)
		{
			rcItem.y = top;
			rcItem.height = (*it)->GetItemHeight();
			break;
		}

		i++;
		top += (*it)->GetItemHeight();
	}

	if(mSelectIndex == i)
	{
		
		if(rc.y+rcItem.y + rcItem.height-mScrollY> rc.y+rc.height)
		{
			mScrollY += (rc.y+rcItem.y + rcItem.height-mScrollY) - (rc.y+rc.height);
		}

		else if(rcItem.y-mScrollY < 0)
		{
			mScrollY += (rcItem.y-mScrollY);
			if(mScrollY <0)
			mScrollY = 0;
		}	

		return;
	}

}

bool DUIListControl::OnKeyEvent(int flag, int ev, int key)
{

	if((ev == ZMAEE_EV_KEY_RELEASE || ev == ZMAEE_EV_KEY_LONGPRESS) && key == ZMAEE_KEY_SOFTOK)
	{
		DUIListItem* pItemSelect = GetDataItem(mSelectIndex);
		if(pItemSelect != NULL)
		{
			DUINotifyEvent nv = {0};
			nv.mOwnerWnd = NULL;
			nv.mSender = this;
			nv.mType = DUI_NOTIFY_LIST_ITEMCLICK;
			if(ev == ZMAEE_EV_KEY_LONGPRESS)
				nv.wParam =UISTYLE_CANLONGTOUCH;

			nv.lParam = (unsigned long)pItemSelect;
			mManager->NotifyEvent(&nv);
			return true;
		}

	}

	else if(ev == ZMAEE_EV_KEY_RELEASE && (key == ZMAEE_KEY_UP || key == ZMAEE_KEY_DOWN))
	{
		int nOldSelect = mSelectIndex;
		if(mDataItems.size() > 0)
		{
			if(key == ZMAEE_KEY_UP)
				mSelectIndex--;
			else
				mSelectIndex++;

			if(mSelectIndex < 0)
			{
				mSelectIndex = 0;
				if(mPageIndex > 0)
				{
					DUINotifyEvent nv = {0};
					nv.mOwnerWnd = NULL;
					nv.mSender = this;
					nv.mType = DUI_NOTIFY_LIST_GETPREPAGE;
					nv.wParam = mPageIndex - 1;
					nv.lParam = mPageSize;
					mManager->NotifyEvent(&nv);
				}
			}
			else if(mSelectIndex >= mDataItems.size())
			{
				mSelectIndex = mDataItems.size() - 1;
				if(mUsePager && mPageIndex * mPageSize + mDataItems.size() < mPageTotal)
				{
					DUINotifyEvent nv = {0};
					nv.mOwnerWnd = NULL;
					nv.mSender = this;
					nv.mType = DUI_NOTIFY_LIST_GETNEXTPAGE;
					nv.wParam = mPageIndex + 1;
					nv.lParam = mPageSize;
					mManager->NotifyEvent(&nv);
				}
			}

			if(nOldSelect != mSelectIndex) {
				MakeSelectItemVisible();
				GetManager()->PostRepaintMessage();
			}

			return true;
		}
	}
	return false;
}

TRect DUIListControl::GetWindowRect()
{
	TRect rc = GetRect();
	DUIControl* pParent = GetParent();
	while(pParent)
	{
		rc.x += pParent->GetRect().x;
		rc.y += pParent->GetRect().y;
		
		if(pParent->IsContainer())
		{
			DUIScrollInfo dif;
			pParent->GetScrollInfo(dif);
			rc.x -= dif.pos_x;
			rc.y -= dif.pos_y;
		}
		
		pParent = pParent->GetParent();
	}
	
	if(mUsePager) rc.y -= mScrollY;

	return rc;
}

int DUIListControl::HitTest(int posx, int posy)
{
	TRect rc = GetWindowRect();
	int top = rc.y-mScrollY;
	int i = 0;
	AEEList<DUIListItem*>::iterator it;
	for(it = mDataItems.begin(); it != mDataItems.end(); ++it)
	{
		if(posy >= top && posy <= top+(*it)->GetItemHeight())
			return i;
		i++;
		top += (*it)->GetItemHeight();
	}
	return -1;
}

bool DUIListControl::OnMotionEvent(const DUIMotionEvent* pMotionEv)
{
	TRect rc;
	switch(pMotionEv->GetAction(0))
	{
	case DUIMotionEvent::ACTION_DOWN:
		if(IsScreenVisible(&rc) && rc.PtInRect(pMotionEv->GetX(0), pMotionEv->GetY(0)))
		{
			mManager->SetPtDownControl(this);
			if(CheckStyle(UISTYLE_CANLONGTOUCH))
			{
				if(mTimerID != -1)
					AEE_IShell_CancelTimer(ZMAEE_GetShell(),mTimerID);
				mTimerID = -1 ;
				mTimerID =  AEE_IShell_SetTimer(ZMAEE_GetShell(),mLongTouchIvl,LongTouchCb,1,this);
			}

			mPenDownIndex = HitTest(pMotionEv->GetX(0), pMotionEv->GetY(0));

			mManager->PostRepaintMessage();
		}
		break;

	case DUIMotionEvent::ACTION_UP:
		if(mTimerID != -1)
			AEE_IShell_CancelTimer(ZMAEE_GetShell(),mTimerID);
		mTimerID = -1 ;

		if(	mManager->GetPtDownControl() == this )
		{
			mManager->SetPtDownControl(NULL);
			if(	IsScreenVisible(&rc) &&  rc.PtInRect(pMotionEv->GetX(0), pMotionEv->GetY(0)))
			{
				int nUpItemIndex = HitTest(pMotionEv->GetX(0), pMotionEv->GetY(0));
				if(nUpItemIndex == mPenDownIndex && mPenDownIndex != -1)
				{
					mPenDownIndex = -1;
					mManager->PostRepaintMessage();

					DUINotifyEvent nv;
					nv.mOwnerWnd = NULL;
					nv.mSender = this;
					nv.mType = DUI_NOTIFY_LIST_ITEMCLICK;
					nv.lParam = (unsigned long)GetDataItem(nUpItemIndex);
					mManager->SetNotifyedCtrl(this);
					mManager->NotifyEvent(&nv);
					return true;
				}
			}
		}
		break;
	}

	return false;
}

void DUIListControl::SetScrollPos(int x, int y)
{

	mScrollY = y;
}

void DUIListControl::GetScrollInfo(DUIScrollInfo& info)
{
	TRect rcClient = GetClientRect();
	info.pos_x = 0;
	info.pos_y = mScrollY;
	info.min_x = 0;
	info.min_y = 0;
	info.max_x = 0;
	int dy = mContentHeight - rcClient.height;
	info.max_y = dy>0?dy:0;
	info.page_w = rcClient.width;
	info.page_h = rcClient.height;
}

void DUIListControl::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{

	MakeSelectItemVisible();
	DUIContainer* pParent = (DUIContainer*)this->GetParent();
	if(mDataItems.size() == 0)
		return;
	TRect rcContectClip =  GetClientRect();
	TRect rcContainer = GetClientRect();
	rcContectClip.x += mLeft+nParentX ;
	rcContectClip.y += mTop+nParentY ;

	rcContainer.x += mLeft + nParentX;
	rcContainer.y += mTop + nParentY - mScrollY;
	
	if(mSelectIndex >= mDataItems.size()) mSelectIndex = mDataItems.size() - 1;
	//if(mSelectIndex < 0) mSelectIndex = 0;
	if(mManager->GetPtDownControl() == this && mSelectIndex != mPenDownIndex)
	{
		mSelectIndex = 	mPenDownIndex;
	}
	else
	{
		mPenDownIndex = mSelectIndex;
	}
	TRect childRc = rcContainer;
	int i = 0;
	AEEList<DUIListItem*>::iterator it;
	for(it = mDataItems.begin(); it != mDataItems.end(); ++it)
	{
		childRc.height = (*it)->GetItemHeight();
		int nSelected = 0;
		
		if( (GetManager()->IsSupportKey() && i == mSelectIndex  )	|| (mManager->GetPtDownControl() == this && i == mPenDownIndex))
			nSelected = 1;
		else
			nSelected = 0;

		if(pDC->PushAndSetIntersectClip(rcContectClip.x, rcContectClip.y, rcContectClip.width,rcContectClip.height))
		{
			(*it)->OnDraw(pDC, nSelected, childRc.x, childRc.y, childRc.width, childRc.height);
			pDC->PopAndRestoreClipRect();
		}

		childRc.y += childRc.height;
		++i;
	}
}


int DUIListControl::GetDataItemCount()
{
	return mDataItems.size();
}

void DUIListControl::SetPageParam(int nTotal, int nPageSize, int nPageIndex)
{
	mPageTotal = nTotal;
	mPageSize = nPageSize;
	mPageIndex = nPageIndex;
}

void DUIListControl::RemoveDataItem(int idx, bool bRepaint)
{
	int i = 0;
	if(idx < 0 || idx >= mDataItems.size())
		return;
	
	for(AEEList<DUIListItem*>::iterator it = mDataItems.begin(); it != mDataItems.end(); ++it){
		if(i == idx) 
		{
			mContentHeight += (*it)->GetItemHeight();
			delete (*it);
			mDataItems.erase(it);
			break;
		}
		++i;
	}

	if(mSelectIndex >= mDataItems.size() && mDataItems.size() > 0)
		mSelectIndex = mDataItems.size() - 1;

	if(bRepaint) GetManager()->PostRepaintMessage();
}


void DUIListControl::RemoveDataItem(DUIListItem* pItem,bool bRepaint)
{
	for(AEEList<DUIListItem*>::iterator it = mDataItems.begin(); it != mDataItems.end(); ++it)
	{
		if(pItem == (DUIListItem*)(*it))
		{
			mContentHeight += (*it)->GetItemHeight();
			delete (*it);
			mDataItems.erase(it);
			break;
		}
	}
	
	if(mSelectIndex >= mDataItems.size() && mDataItems.size() > 0)
		mSelectIndex = mDataItems.size() - 1;

	if(bRepaint) GetManager()->PostRepaintMessage();
}

void DUIListControl::RemoveDataAll()
{

	mContentHeight = 0;
	mScrollY = 0;
	for(AEEList<DUIListItem*>::iterator it = mDataItems.begin(); it != mDataItems.end(); ++it)
		delete (*it);

	mDataItems.clear();
}
void DUIListControl::PushBackDataItem(DUIListItem* pItem, bool bRepaint)
{
	mDataItems.push_back(pItem);
	if(mUsePager && mDataItems.size() > mMaxDataItem && mMaxDataItem > 0)
	{
		DUIListItem* pItem = mDataItems.front();
		if(mUsePager) {
			if(mScrollY < 0) mScrollY += pItem->GetItemHeight();
			mSelectIndex--;
			if(mSelectIndex < 0) mSelectIndex = 0;
		}
		delete pItem;
		mDataItems.pop_front();
	}
	mContentHeight += pItem->GetItemHeight();

	if(bRepaint) GetManager()->PostRepaintMessage();
}

void DUIListControl::PushFrontDataItem(DUIListItem* pItem, bool bRepaint)
{
	mDataItems.push_front(pItem);
	mContentHeight += pItem->GetItemHeight();
	if(mUsePager && mDataItems.size() > mMaxDataItem && mMaxDataItem > 0)
	{
		DUIListItem* pItemBack = mDataItems.back();
		delete pItemBack;
		mDataItems.pop_back();
		if(mUsePager) {
			mScrollY -= pItem->GetItemHeight();
			mSelectIndex++;
			if(mSelectIndex >= mDataItems.size()) mSelectIndex = mDataItems.size()-1;
		}
	}

	if(bRepaint) GetManager()->PostRepaintMessage();
}

void DUIListControl::PopFront( bool bRepaint)
{
	DUIListItem* pItemBack = mDataItems.front();
	if(pItemBack)
	{
		mScrollY -= pItemBack->GetItemHeight();
		mContentHeight -= pItemBack->GetItemHeight();
		delete pItemBack;
		mDataItems.pop_front();	
	
		if(bRepaint)
			GetManager()->PostRepaintMessage();
			
	}
		
}

AEEList<DUIListItem*>::iterator  DUIListControl::GetDataItemInter(int nIdx)
{
	int i = 0;
	AEEList<DUIListItem*>::iterator it;
	for(it = mDataItems.begin(); it != mDataItems.end(); ++it )
	{
		if(i == nIdx)
			return it;
		++i;
	}
	return it;
}

DUIListItem* DUIListControl::GetDataItem(int nIdx)
{
	AEEList<DUIListItem*>::iterator it = GetDataItemInter(nIdx);
	return (it==mDataItems.end()) ? 0 : (*it);
}


void DUIListControl::LongTouchCb(int timerID,void* pUser)
{


	DUIListControl* pThis = (DUIListControl*)pUser;
	if(pThis)
	{
		if(pThis->mTimerID >=0)
			AEE_IShell_CancelTimer(ZMAEE_GetShell(),pThis->mTimerID);
		pThis->mTimerID = -1;

		DUIManager* pManager = pThis->GetManager();
		if(pThis->mPenDownIndex != -1)
		{
		
			pManager->PostRepaintMessage();
			DUINotifyEvent nv;
			nv.mOwnerWnd = NULL;
			nv.mSender = pThis;
			nv.mType = DUI_NOTIFY_LIST_ITEMCLICK;
			nv.wParam = UISTYLE_CANLONGTOUCH;
			nv.lParam = (unsigned long)pThis->GetDataItem(pThis->mPenDownIndex);
			if(nv.lParam )
			{
				pManager->SetNotifyedCtrl(pThis);
				pManager->NotifyEvent(&nv);
			}
			pThis->mPenDownIndex = -1;
		}
	}
	
}