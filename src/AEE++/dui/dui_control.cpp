#include "dui_control.h"
#include "dui_dc.h"
#include "dui_bdfiller.h"
#include "dui_bgfiller.h"
#include "dui_manager.h"
#include "dui_stringutil.h"
#ifdef WIN32
#include "stdio.h"
#endif


DUIControl::DUIControl()
{
	mClassID = DUIStringUtil::StrHash("control");
	mID = 0;
	mManager = NULL;
	mParent = NULL;
	mBorderFiller = NULL;
	mBgFiller = NULL;
	mFocusBgFiller = NULL;
	mFocusBorderFiller = NULL;
	mPushBgFiller = NULL;
	mUserData = NULL;
	mPushBorderFiller = NULL;
	mStyle = UISTYLE_VISIBLE;
	mRelativePos = NULL;
	mWidth = 0;
	mHeight = 0;
	mLeft = 0;
	mTop = 0;
	mDefaultFocusStyle = false;
	mLongTouchIvl = 1000;
	mTimerID = -1;
	
	mAlign = 0;

}

DUIControl::~DUIControl()
{
	
	if(mTimerID >=0)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(),mTimerID);
	mTimerID = -1;
	if(mRelativePos)
		delete mRelativePos;
	if(mBorderFiller)
		mBorderFiller->Release();
	if(mBgFiller)
		mBgFiller->Release();
	if(mFocusBgFiller)
		mFocusBgFiller->Release();
	if(mFocusBorderFiller)
		mFocusBorderFiller->Release();
	if(mPushBgFiller)
		mPushBgFiller->Release();
	if(mPushBorderFiller)
		mPushBorderFiller->Release();
}

void DUIControl::SetParent(DUIControl* pParent)
{
	mParent = pParent;
}

void DUIControl::SetManager(DUIManager* pManager)
{
	mManager = pManager;
	if(pManager)
	{
		mDefaultFocusStyle = pManager->IsSupportKey();
	}
}

void DUIControl::SetClass(const AEEStringRef& szName)
{
	mClassID = DUIStringUtil::StrHash(szName);
}

bool DUIControl::IsClass(const AEEStringRef& szName)
{
	return (mClassID == DUIStringUtil::StrHash(szName));
}

bool DUIControl::IsContainer() const
{
	return false;
}

TBorder DUIControl::GetBorder()
{
	TBorder bd;
	DUIBorderFiller* pBorderFiller = GetCurBorder();
	if(pBorderFiller)
		bd = pBorderFiller->Border();
	return bd;
}

void DUIControl::SetUserData(void* pUserData)
{
	mUserData = pUserData;
}

void* DUIControl::GetUserData(void)
{
	return mUserData;
}

TSize DUIControl::MeasureSize(int nParentCx, int nParentCy)
{
	return TSize(
		mPercentWidth.IsValid() ? mPercentWidth.calc(nParentCx) : mWidth, 
		mPercentHeight.IsValid() ? mPercentHeight.calc(nParentCy) : mHeight );
}

bool DUIControl::OnKeyEvent(int flag, int ev, int key)
{
	if(ev == ZMAEE_EV_KEY_RELEASE && ((key == ZMAEE_KEY_SOFTOK) || (key == ZMAEE_KEY_SOFTRIGHT) || (key == ZMAEE_KEY_UP) || (key == ZMAEE_KEY_DOWN)))
	{
		DUINotifyEvent nv = {0};
		nv.mOwnerWnd = NULL;
		nv.mSender = this;
	//	nv.mType = DUI_NOTIFY_CLICK;
		nv.mType = DUI_NOTIFY_KEYBORD;
	//	nv.wParam = 0;
		nv.wParam = key;
		nv.lParam = 0;
		mManager->NotifyEvent(&nv);
		return true;
	}

	return false;
}

bool DUIControl::OnMotionEvent(const DUIMotionEvent* pMotionEv)
{
	if(!CheckStyle(UISTYLE_CANCLICK|UISTYLE_VISIBLE))
		return false;

	TRect rc;
	switch(pMotionEv->GetAction(0))
	{
	case DUIMotionEvent::ACTION_DOWN:
		if(IsScreenVisible(&rc) && rc.PtInRect(pMotionEv->GetX(0), pMotionEv->GetY(0)))
		{
			ModifyStyle(UISTYLE_PUSHED, 0);
			mManager->SetPtDownControl(this);
		
			if(CheckStyle(UISTYLE_CANLONGTOUCH))
			{
				if(mTimerID >=0)
					AEE_IShell_CancelTimer(ZMAEE_GetShell(),mTimerID);
				mTimerID =  AEE_IShell_SetTimer(ZMAEE_GetShell(),mLongTouchIvl,LongTouchCb,1,this);
			}
			if(mPushBgFiller || mPushBorderFiller)
				mManager->PostRepaintMessage();
		}
		break;

	case DUIMotionEvent::ACTION_UP:
		if(mTimerID >=0)
			AEE_IShell_CancelTimer(ZMAEE_GetShell(),mTimerID);
		mTimerID = -1;
		
		if(	mManager->GetPtDownControl() == this )
		{
			ModifyStyle(0, UISTYLE_PUSHED);
			mManager->SetPtDownControl(NULL);
			if( mPushBgFiller || mPushBorderFiller )
				mManager->PostRepaintMessage();
			
			if(	IsScreenVisible(&rc) && 
				rc.PtInRect(pMotionEv->GetX(0), pMotionEv->GetY(0)))
			{
				if(CheckStyle(UISTYLE_CHECKED))
					ModifyStyle(0, UISTYLE_CHECKED);
				else
					ModifyStyle(UISTYLE_CHECKED, 0);
				
				mManager->PostRepaintMessage();
				
				DUINotifyEvent nv;
				nv.mOwnerWnd = NULL;
				nv.mSender = this;
				nv.mType = DUI_NOTIFY_CLICK;
				nv.wParam = pMotionEv->GetX(0);
				nv.lParam = pMotionEv->GetY(0);
				mManager->SetNotifyedCtrl(this);
				mManager->NotifyEvent(&nv);
			
				return true;
			}
		}
		else
			mManager->SetNotifyedCtrl(NULL);
		break;
	}
	return false;
}

TRect DUIControl::GetRect()
{
	TRect rc(mLeft, mTop, mWidth, mHeight);
	return rc;
}

TRect DUIControl::GetClientRect()
{
	TRect rc(0, 0, mWidth, mHeight);
	rc -= GetMargin();
	rc -= GetBorder();
	rc -= GetPadding();
	return rc;
}

void DUIControl::SetRect(int x, int y, int cx, int cy)
{
	if(cx < 0 || cy < 0)
	{
		TRect rcParent = mParent->GetRect();
		cx = rcParent.width;
		cy = rcParent.height;
		x = rcParent.x;
		y = rcParent.y;
	}

	if(mRelativePos)
	{
		TSize sizeChild = MeasureSize(cx, cy);
		TPoint pos = mRelativePos->GetPostion(x, y, cx, cy);
		mLeft = pos.x;
		mTop = pos.y;
		mWidth = sizeChild.cx;
		mHeight = sizeChild.cy;
	}
	else
	{
		mLeft = x;
		mTop = y;
		mWidth = cx;
		mHeight = cy;
	}
}



void DUIControl::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_NAME:
		mID = DUIStringUtil::StrHash(szValue);
		break;
	case DUI_ATTRIBUTE_ALIGN:
		mAlign = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_VISIABLE:
		if(DUIStringUtil::ParseBool(szValue))
			mStyle |= UISTYLE_VISIBLE;
		else
			mStyle &= ~UISTYLE_VISIBLE;
		break;
	case DUI_ATTRIBUTE_CANFOCUS:
		if(DUIStringUtil::ParseBool(szValue))
			mStyle |= UISTYLE_CANFOCUS;
		else
			mStyle &= ~UISTYLE_CANFOCUS;
		break;
	case DUI_ATTRIBUTE_FOCUS:
		if(DUIStringUtil::ParseBool(szValue))
			mStyle |= UISTYLE_FOCUSED;
		else
			mStyle &= ~UISTYLE_FOCUSED;
		break;
	case DUI_ATTRIBUTE_BACKGROUND:
		check_release_bg_filler(mBgFiller);
		{
			int nBgId = DUIStringUtil::ParseInt(szValue);
			mBgFiller = mManager->GetBGFiller(nBgId);
		}
		break;
	case DUI_ATTRIBUTE_PUSHGROUND:
		check_release_bg_filler(mPushBgFiller);
		{
			int nBgId = DUIStringUtil::ParseInt(szValue);
			mPushBgFiller = mManager->GetBGFiller(nBgId);

			if(mDefaultFocusStyle && mFocusBgFiller == NULL)
				mFocusBgFiller = mManager->GetBGFiller(nBgId);
		}
		break;
	case DUI_ATTRIBUTE_FOCUSGROUND:
		check_release_bg_filler(mFocusBgFiller);
		{
			int nBgId = DUIStringUtil::ParseInt(szValue);
			mFocusBgFiller = mManager->GetBGFiller(nBgId);
		}
		break;
	case DUI_ATTRIBUTE_BORDER:
		check_release_border_filler(mBorderFiller);
		{
			int nId = DUIStringUtil::ParseInt(szValue);
			mBorderFiller = mManager->GetBorderFiller(nId);
		}
		break;
	case DUI_ATTRIBUTE_PUSHBORDER:
		check_release_border_filler(mPushBorderFiller);
		{
			int nId = DUIStringUtil::ParseInt(szValue);
			mPushBorderFiller = mManager->GetBorderFiller(nId);

			if(mDefaultFocusStyle && mFocusBorderFiller == NULL)
				mFocusBorderFiller = mManager->GetBorderFiller(nId);
		}
		break;
	case DUI_ATTRIBUTE_FOCUSBORDER:
		check_release_border_filler(mFocusBorderFiller);
		{
			int nId = DUIStringUtil::ParseInt(szValue);
			mFocusBorderFiller = mManager->GetBorderFiller(nId);
		}
		break;

	case DUI_ATTRIBUTE_POS_TYPE:
		{
			int nType = DUIStringUtil::ParseInt(szValue);
			if(nType == 0)
			{
				if(mRelativePos) delete mRelativePos;
				mRelativePos = NULL;
			}
			else
			{
				if(mRelativePos == NULL)
					mRelativePos = new TRelativePos();
				mRelativePos->type = DUIStringUtil::ParseInt(szValue);
			}
		}
		break;
	case DUI_ATTRIBUTE_POS_X:
		{
			if(mRelativePos)
				mRelativePos->x = DUIStringUtil::ParseInt(szValue);
			else
				mLeft = DUIStringUtil::ParseInt(szValue);
		}
		break;
	case DUI_ATTRIBUTE_POS_Y:
		{
			if(mRelativePos)
				mRelativePos->y = DUIStringUtil::ParseInt(szValue);
			else
				mTop = DUIStringUtil::ParseInt(szValue);
		}
		break;
	case DUI_ATTRIBUTE_WIDTH:
		{
			int width = DUIStringUtil::ParseInt(szValue);
			if(szValue.endwith("%"))
				mPercentWidth.SetPercent(width);
			else
				mWidth = width;
		}
		break;
	case DUI_ATTRIBUTE_HEIGHT:
		{
			int height = DUIStringUtil::ParseInt(szValue);
			if(szValue.endwith("%"))
				mPercentHeight.SetPercent(height);
			else if(szValue == "auto")
				mPercentHeight.SetAuto();
			else
			{
				mPercentHeight.valid = 0;
				mHeight = height;
			}
				
		}
		break;
	case DUI_ATTRIBUTE_MARGIN:
		{
			TRect rc = DUIStringUtil::ParseRect(szValue);
			mMargin.set(rc.x, rc.y , rc.width, rc.height);
		}
		break;
	case DUI_ATTRIBUTE_MARGIN_LEFT:
		mMargin.left = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_MARGIN_TOP:
		mMargin.top = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_MARGIN_RIGHT:
		mMargin.right = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_MARGIN_BOTTOM:
		mMargin.bottom = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_PADDING:
		{
			TRect rc = DUIStringUtil::ParseRect(szValue);
			mPadding.set(rc.x, rc.y , rc.width, rc.height);
		}
		break;
	case DUI_ATTRIBUTE_PADDING_LEFT:
		mPadding.left = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_PADDING_TOP:
		mPadding.top = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_PADDING_RIGHT:
		mPadding.right = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_PADDING_BOTTOM:
		mPadding.bottom = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_CANLONG_TOUCH:
		if(DUIStringUtil::ParseBool(szValue))
			mStyle |= UISTYLE_CANLONGTOUCH;
		else
			mStyle &= ~UISTYLE_CANLONGTOUCH;
		break;
	case DUI_ATTRIBUTE_LONGTOUCH_INTERVAL:
		mLongTouchIvl = DUIStringUtil::ParseInt(szValue);
		break;
	default:
		break;
	}
}

DUIBorderFiller* DUIControl::GetCurBorder()
{
	if(CheckStyle(UISTYLE_PUSHED) && mPushBorderFiller)
		return mPushBorderFiller;

	if(CheckStyle(UISTYLE_FOCUSED) && mFocusBorderFiller)
		return mFocusBorderFiller;

	return mBorderFiller;
}

DUIBGFiller* DUIControl::GetCurBackground()
{
	if(CheckStyle(UISTYLE_PUSHED) && mPushBgFiller)
		return mPushBgFiller;

	if(CheckStyle(UISTYLE_FOCUSED) && mFocusBgFiller)
		return mFocusBgFiller;

	return mBgFiller;
}

TRect DUIControl::GetVirtualRect()
{
	TRect rc = GetClientRect();
	rc.x = rc.y = 0;
	return rc;
}

void  DUIControl::SetScrollPos(int /*x*/, int /*y*/)
{
}

void DUIControl::GetScrollInfo(DUIScrollInfo& info)
{
	info.pos_x = 0;
	info.pos_y = 0;
	info.min_x = info.min_y = 0;
	info.max_x = info.max_y = 0;
	info.page_h = info.page_w = 0;
}


TRect DUIControl::GetAbsoluteRect()
{
	TRect rc = GetRect();
	DUIControl* pParent = mParent;
	while(pParent != NULL)
	{
		
		rc.x += pParent->mLeft;
		rc.y += pParent->mTop;
		pParent = pParent->mParent;
	}
	return rc ;
	
}

bool DUIControl::IsScreenVisible(TRect* pRect)
{
	if(!CheckStyle(UISTYLE_VISIBLE))
		return false;

	TRect rc = GetRect();
	DUIControl* pParent = mParent;
	
	while(pParent != NULL)
	{
		if(!pParent->CheckStyle(UISTYLE_VISIBLE))
			return false;

		TRect rcParent = pParent->GetVirtualRect();
		if(!rcParent.IntersectRect(rc, &rc))
			return false;
		
		int dx = pParent->GetMargin().left + pParent->GetBorder().left + pParent->GetPadding().left;
		int dy = pParent->GetMargin().top + pParent->GetBorder().top + pParent->GetPadding().top;

		rc.x += pParent->mLeft - rcParent.x + dx;
		rc.y += pParent->mTop - rcParent.y + dy;

		pParent = pParent->mParent;
	}

	if(pRect) *pRect = rc;

	return true;
}

void DUIControl::OnPaint(DUIDC* pDC, int nParentX, int nParentY)
{
	if(!CheckStyle(UISTYLE_VISIBLE))
		return;
	if(mRelativePos)
	{
		bool ret = false;
		DUIContainer* pParent = (DUIContainer*)GetParent();
		TRect rcContainer ;
		int x =mLeft+nParentX;
		int y = mTop+nParentY;
		while (pParent)
		{
			rcContainer = GetRect();
			if(pParent->CanScroll())
			{
				ret = true ;
				break ;
			}
			pParent = (DUIContainer*)pParent->GetParent();
		}
		if(ret)
		{
			rcContainer =pParent->GetRect();
		}
		if(y<rcContainer.y)
			y = rcContainer.y;
		if(x<rcContainer.x)
			x = rcContainer.x;
	 //	pDC->PushAndSetClipRect(x, y, mWidth, mHeight);
		pDC->PushAndSetIntersectClip(x, y, mWidth, mHeight);
	}
		
	OnPaintBackground(pDC, nParentX, nParentY);

	OnPaintBorder(pDC, nParentX, nParentY);

	OnPaintStatusImage(pDC, nParentX, nParentY);

	OnPaintContent(pDC, nParentX, nParentY);


	if(mRelativePos) pDC->PopAndRestoreClipRect();

}

void DUIControl::OnPaintBackground(DUIDC* pDC ,int nParentX, int nParentY)
{
	DUIBGFiller* pBgFiller = GetCurBackground();
 	if(pDC && pBgFiller)
 	{
		TRect rect(mLeft + nParentX, mTop + nParentY, mWidth, mHeight);
 		rect -= GetMargin();
		DUIBorderFiller* pBorderFiller = GetCurBorder();
		if(pBorderFiller) rect -= GetBorder();
 		pBgFiller->FillRect(pDC, rect);
 	}
	
}

void DUIControl::OnPaintBorder(DUIDC* pDC , int nParentX, int nParentY)
{

	DUIBorderFiller* pBorderFiller = GetCurBorder();

	if(pDC && pBorderFiller)
	{
		
		TRect rect(mLeft + nParentX, mTop + nParentY, mWidth, mHeight);
	
		rect -= GetMargin();
		
		pBorderFiller->FillRect(pDC, rect);
		
	}
}

void DUIControl::OnPaintStatusImage(DUIDC* /*pDC*/, int /*nParentX*/, int /*nParentY*/)
{


}
void DUIControl::OnPaintContent(DUIDC* /*pDC*/, int /*nParentX*/, int /*nParentY*/)
{

	

}

void DUIControl::LongTouchCb(int timerID,void* pUser)
{
	DUIControl* pCtrl = (DUIControl*)pUser;
	if(pCtrl)
	{
		if(pCtrl->CheckStyle(UISTYLE_PUSHED))
		{	
			DUIManager* pManager = pCtrl->GetManager();
			if(	pManager->GetPtDownControl() == pCtrl )
			{
				pCtrl->ModifyStyle(0, UISTYLE_PUSHED);
				pManager->SetPtDownControl(NULL);

				if(pCtrl->CheckStyle(UISTYLE_CHECKED))
					pCtrl->ModifyStyle(0, UISTYLE_CHECKED);
				else
					pCtrl->ModifyStyle(UISTYLE_CHECKED, 0);
				
				pManager->PostRepaintMessage();
				DUINotifyEvent nv;
				nv.mOwnerWnd = NULL;
				nv.mSender = pCtrl;
				nv.mType = DUI_NOTIFY_LONG_TOUCH;
				pManager->NotifyEvent(&nv);
			}
		}
	}
	
}


