#include "dui_container.h"
#include "dui_stringutil.h"
#include "dui_manager.h"
#include "layout/dui_layout_hbox.h"
#include "layout/dui_layout_vbox.h"
#include "layout/dui_layout_border.h"
#include "layout/dui_layout_grid.h"
#include "layout/dui_layout_relative.h"
#include "layout/dui_layout_tab.h"
#include "layout/dui_layout_vflow.h"
#include "dui_pages.h"


#ifdef WIN32
#include <stdio.h>
#endif

DUIContainer::DUIContainer()
{
	mClrFmt = ZMAEE_COLORFORMAT_16;
	mLayout = NULL;
	mLayerId = -1;
	mUseLayer = false;
	mContentWidth = 0;
	mContentHeight = 0;
	mScrollX = 0;
	mScrollY = 0;
	mClassID = DUIStringUtil::StrHash("container");
	mFocus = NULL;
	mCanScroll = false;
}

DUIContainer::~DUIContainer()
{
	if(mLayout)
		delete mLayout;
	mLayout = NULL;

	if(mLayerId > 0)
		mManager->GetDC()->FreeLayer(mLayerId);

	mLayerId = -1;
	RemoveAll();
}

bool DUIContainer::IsContainer() const
{
	return true;
}

TRect DUIContainer::GetVirtualRect()
{
	TRect rcClient = GetClientRect();
	rcClient.x += mScrollX;
	rcClient.y += mScrollY;
	return rcClient;
}

bool DUIContainer::OnMotionEvent(const DUIMotionEvent* pMotionEv)
{
//	PRINTF("DUIContainer::OnMotionEvent---------start\n");
	if(CheckStyle(UISTYLE_CANCLICK))
	{
		return DUIControl::OnMotionEvent(pMotionEv);
	}

	DUIControls::iterator it;
//	PRINTF("DUIContainer::OnMotionEvent:mItems.size=%d\n",mItems.size());
	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		
		if((*it)->IsScreenVisible(NULL))
		{
			DUIControl* pItem = (DUIControl*)(*it);
			if((*it)->OnMotionEvent(pMotionEv))
				return true;
		}
	}
	return false;
}

void DUIContainer::SetScrollPos(int x, int y)
{
	mScrollX = x;
	mScrollY = y;
}

void DUIContainer::GetScrollInfo(DUIScrollInfo& info)
{
	TRect rcClient = GetClientRect();
	info.pos_x = mScrollX;
	info.pos_y = mScrollY;
	info.min_x = 0;
	info.min_y = 0;
	info.max_x = mContentWidth - rcClient.width;
	info.max_y = mContentHeight - rcClient.height;
	info.page_w = rcClient.width;
	info.page_h = rcClient.height;
}

void DUIContainer::OnPaint(DUIDC* pDC, int nParentX, int nParentY)
{
	if(!CheckStyle(UISTYLE_VISIBLE) || !IsScreenVisible())
		return;
	
	if(mUseLayer && (mLayerId < 0 || !pDC->IsValidLayer(mLayerId)))
	{
		TRect rc(mLeft + nParentX, mTop + nParentY, mWidth, mHeight);
		mLayerId = pDC->CreateLayer(&rc, mClrFmt);
		ZMAEE_LayerInfo li ;
		if(pDC->GetLayerInfo(mLayerId,&li))
		{
			if(li.cf == ZMAEE_COLORFORMAT_32)
				zmaee_memset(li.pFrameBuf,0,li.nWidth*li.nHeight*4);
			else if(li.cf == ZMAEE_COLORFORMAT_16)
				zmaee_memset(li.pFrameBuf,0,li.nWidth*li.nHeight*2);
		}
		#ifdef WIN32
		if(mLayerId < 0)
		{
			printf("DUILayer::OnPaint create layer[w=%d,h=%d,clrFmt=%d] failure\n", mWidth, mHeight, mClrFmt);
			return;
		}
		#endif
	}

	int oldActiveLayer=0;
	if(mUseLayer && mLayerId > 0) 
	{
		oldActiveLayer= pDC->GetActiveLayer();
		ZMAEE_LayerInfo li ;
		if(pDC->GetLayerInfo(mLayerId,&li))
		{
			if(li.cf == ZMAEE_COLORFORMAT_32)
				zmaee_memset(li.pFrameBuf,0,li.nWidth*li.nHeight*4);
		}
		pDC->SetActiveLayer(mLayerId);
	}
	
	pDC->PushAndSetIntersectClip(mLeft + nParentX, mTop + nParentY, mWidth, mHeight);
	DUIControl::OnPaint(pDC, nParentX, nParentY);
	pDC->PopAndRestoreClipRect();

	if(mUseLayer && mLayerId > 0) {
		pDC->SetActiveLayer(oldActiveLayer);
	}
}

void DUIContainer::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned long dwNameHash = DUIStringUtil::StrHash(szName);
	switch(dwNameHash)
	{
		case DUI_ATTRIBUTE_LAYOUT:
		{
			if(mLayout) {
				delete mLayout;
				mLayout = NULL;
			}

			int nLayoutType = zmaee_strtol((char*)szValue.ptr(), 0, 10);
			switch(nLayoutType)
			{
			case DUI_LAYOUT_TYPE_HBOX:
				mLayout = new DUILayoutHBox();
				break;
			case DUI_LAYOUT_TYPE_VBOX:
				mLayout = new DUILayoutVBox();
				break;
			case DUI_LAYOUT_TYPE_TAB:
				mLayout = new DUILayoutTab();
				break;
			case DUI_LAYOUT_TYPE_GRID:
				mLayout = new DUILayoutGrid();
				break;
			case DUI_LAYOUT_TYPE_BORDER:
				mLayout = new DUILayoutBorder();
				break;
			case DUI_LAYOUT_TYPE_RELATIVE:
				mLayout = new DUILayoutRelative();
				break;
			case DUI_LAYOUT_TYPE_VFLOW:
				mLayout = new DUILayoutVFlow();
				break;
			}
		}
		break;

		case DUI_ATTRIBUTE_LAYOUT_PARAM:
		{
			if(mLayout) mLayout->SetParam(szValue);
		}
		break;

		case DUI_ATTRIBUTE_LAYER:
		{
			mClrFmt = zmaee_strtol((char*)szValue.ptr(), 0, 10);
			mUseLayer = true;
		}
		break;
		default:
		DUIControl::SetAttribute(szName, szValue);
		break;
	}
}

void DUIContainer::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	TRect rcContainer = GetRect();
	rcContainer.x += nParentX;
	rcContainer.y += nParentY;

	TRect childRc;
	DUIControls::iterator it;
	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		childRc = (*it)->GetRect();
		childRc.x += rcContainer.x - mScrollX;
		childRc.y += rcContainer.y - mScrollY;
		
		if((*it)->GetRelativePos() == NULL && rcContainer.IntersectRect(childRc))
			(*it)->OnPaint(pDC, rcContainer.x - mScrollX, rcContainer.y - mScrollY);
		else if((*it)->GetRelativePos())
			(*it)->OnPaint(pDC, rcContainer.x, rcContainer.y);
		
	}
}

DUIControl* DUIContainer::GetItemAt(int iIndex)
{
	if(iIndex < 0 || iIndex >= mItems.size())
		return NULL;

	DUIControls::iterator it;
	int i = 0;
	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		if(i == iIndex)
			return (*it);
		i++;
	}

	return NULL;
}

int DUIContainer::GetItemIndex(DUIControl* pControl)
{
	DUIControls::iterator it;
	int i = 0;
	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		if(pControl == *it)
			return i;
		i++;
	}

	return -1;
}

int DUIContainer::GetCount()
{
	return mItems.size();
}

bool DUIContainer::Add(DUIControl* pControl)
{
	pControl->SetParent(this);
	if(pControl->GetManager() == NULL){
		pControl->SetManager(GetManager());
	}
	mItems.push_back(pControl);
	return true;
}

bool DUIContainer::AddAt(DUIControl* pControl, int iIndex)
{
	DUIControls::iterator it;
	int i = 0;

	if(mItems.size()== 0)
	{
		pControl->SetParent(this);
		mItems.push_back(pControl);
		return true;
	}

	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		if(i == iIndex)
		{
			pControl->SetParent(this);
			mItems.insert(it, pControl);
			return true;
		}
		i++;
	}

	return false;
}

bool DUIContainer::Remove(DUIControl* pControl, bool bDestroy)
{
	DUIControls::iterator it;
	it = mItems.find(pControl);

	if(it != mItems.end())
	{
		mItems.erase(it);
		if(bDestroy)
			delete pControl;
		return true;
	}

	return false;
}

bool DUIContainer::RemoveAt(int iIndex)
{
	DUIControls::iterator it;
	int i = 0;

	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		if(i == iIndex)
		{
			delete *it;
			mItems.erase(it);
			return true;
		}
		i++;
	}

	return false;
}

void DUIContainer::RemoveAll()
{
	for(DUIControls::iterator it = mItems.begin(); it!=mItems.end(); ++it)
		delete *it;
	mItems.clear();
}

void DUIContainer::SetRect(int x, int y, int cx, int cy)
{
	DUIControl::SetRect(x, y, cx, cy);
	if(mLayout == NULL)
		mLayout = new DUILayoutHBox();
	mLayout->doLayout(this, mContentWidth, mContentHeight);
}

void DUIContainer::RefreshLayout()
{
	if(mLayout)
		mLayout->doLayout(this, mContentWidth, mContentHeight);
}

DUIControl* DUIContainer::GetChild(unsigned int nNameId)
{
	DUIControls::iterator it;
	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		if((*it)->GetID() == nNameId)
			return *it;
	}
	return NULL;
}

DUIControl* DUIContainer::GetChild(const char* szChildName)
{
	unsigned int nNameId = DUIStringUtil::StrHash(szChildName);
	return GetChild(nNameId);
}

DUIControl* DUIContainer::GetDepthChild(const char* szName)
{
	DUIContainer* pParent = this;
	DUIControl* pChild = NULL;
	const char* pName = szName;
	const char* pEnd = NULL;

	while((pEnd = zmaee_strstr(pName, ":")) != NULL)
	{
		char tmpName[32] ={0};
		zmaee_memcpy(tmpName, (void*)pName, pEnd-pName);
		pChild = pParent->GetChild(tmpName);
		if(pChild == NULL)
			return NULL;

		pName = pEnd+1;
		pParent = (DUIContainer*)pChild;
	}

	pChild = pParent->GetChild(pName);

	return pChild;
}


bool DUIContainer::PrevFocusCallback(DUIControl* pCtrl, int param)
{
	FocusParam* p_param = (FocusParam*)param;
	if(pCtrl == (DUIControl*)((FocusParam*)param)->mContainer 
		|| pCtrl == NULL
		||pCtrl->CheckStyle(UISTYLE_VISIBLE) == false
		||pCtrl->CheckStyle(UISTYLE_CANFOCUS) == false)
		return false;
		
	if(pCtrl == p_param->mFocus )
		return true;
	
	p_param->mNext = pCtrl;

	return false;
}


bool DUIContainer::NextFocusCallback(DUIControl* pCtrl, int param)
 {
	 FocusParam* p_param = (FocusParam*)param;
	 
	 if(pCtrl == (DUIControl*)(p_param)->mContainer 
		 || pCtrl == NULL 
		 ||pCtrl->CheckStyle(UISTYLE_VISIBLE) == false
		 ||pCtrl->CheckStyle(UISTYLE_CANFOCUS) == false)
		 return false;

	 p_param = (FocusParam*)param;
	 
	 if(p_param->mFocus == NULL)
	 {
		 p_param->mNext = pCtrl;
		 return true;
	 }
	 if(p_param->mFocus == pCtrl)
		 p_param->mFocus = NULL;

	 return false;
 }

DUIControl * DUIContainer:: FindChild(int param,FindCtrlCallback callback)
{
	if(callback == NULL || (mStyle&UISTYLE_VISIBLE) != UISTYLE_VISIBLE)
		return NULL;

	DUIControls::iterator it;
	for(it = mItems.begin();it != mItems.end();it++)
	{
		DUIControl* pCtrl = (DUIControl*)(*it);
		if(((DUIControl*)(*it))->CheckStyle(UISTYLE_VISIBLE))
		{
			if(pCtrl->CheckStyle(UISTYLE_CANFOCUS))
			{
				if(callback((DUIControl*)(*it), param))
					return (DUIControl*)(*it);
			}
			else if(pCtrl->GetClassId() == DUIStringUtil::StrHash("container"))
			{
				DUIControl* pFind = ((DUIContainer*)(*it))->FindChild(param, callback);
				if(pFind)
					return pFind ;
			}
			else if (pCtrl->GetClassId() == DUIStringUtil::StrHash("pages"))
			{
				int curIdx = ((DUIPages*)pCtrl)->GetCurIdx();
				if(curIdx >=0 && curIdx<((DUIPages*)pCtrl)->GetPageCount())
				{
					int idx = 0;
					DUIControls& items = ((DUIContainer*)pCtrl)->GetItems();
					DUIControls::iterator itP ;
					for(itP = items.begin();itP!= items.end();itP++)
					{
						if(idx == curIdx)
						{
							DUIControl* pFind = ((DUIContainer*)(*itP))->FindChild(param, callback);
							if(pFind)
								return pFind ;
							break ;
						}
						idx++;				
					}

				}
				
			}
		}
		
	}

	return NULL;
}
DUIControl*	 DUIContainer::GetNextFocusCtrl(bool forward)
{
	FocusParam param;

	DUIControl* focus = NULL;
	param.mContainer = this;
	param.mFocus = mFocus;
	param.mNext = NULL;

	focus = mFocus;
		
	FindChild((int)&param, forward ? PrevFocusCallback :NextFocusCallback);
	
	if(param.mNext && focus != param.mNext)
	{
		SetFocusCtrl(param.mNext);

		return param.mNext;
	}

	return NULL;
}


bool  DUIContainer::SetScrollbarByFocus(DUIControl* pFocus)
{
	DUIControl* parent = NULL;
	if(!pFocus)
		return true;
	parent = pFocus->GetParent();
	while(parent)
	{
		if((parent->GetClassId() == DUIStringUtil::StrHash("container"))
			&& ((DUIContainer*)parent)->CanScroll())
		{
			DUIContainer* pParent = (DUIContainer*)parent;
			TRect ctrlRect =  pFocus->GetAbsoluteRect();
			TRect parentRect = parent->GetAbsoluteRect();
			DUIScrollInfo info = {0};
			pParent->GetScrollInfo(info);

			int offset = 0;

			if(ctrlRect.y < parentRect.y+info.pos_y)
			{
				offset = ctrlRect.y - (parentRect.y+info.pos_y);
				pParent->SetScrollPos(info.pos_x,info.pos_y + offset);

				return true ;
			}
			else if(ctrlRect.y + ctrlRect.height > parentRect.y+info.pos_y + parentRect.height)
			{
				offset = ctrlRect.y + ctrlRect.height - (parentRect.y+info.pos_y + parentRect.height);
				pParent->SetScrollPos(info.pos_x,info.pos_y+offset);
				return true;
			}
		}
		
		parent = parent->GetParent();
	}

	return false ;
}


bool DUIContainer::SetFocusCtrl(DUIControl* pFocus)
{
	bool ret = true ;
	if(mFocus)
	{
		mFocus->ModifyStyle(0,UISTYLE_FOCUSED);
	}
	mFocus = pFocus;
	if(mFocus)
	{
		mFocus->ModifyStyle(UISTYLE_FOCUSED,0);
		ret = SetScrollbarByFocus(mFocus);
	}
	return ret ;
}

void DUIContainer::SetLayerId(int layerID)
{
	DUIDC* pDC = GetManager()->GetDC();
	if(pDC && pDC->IsValidLayer(layerID))
	{
		mUseLayer = true ;
		mLayerId = layerID;
	}
	
}