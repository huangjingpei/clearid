#include "dui_window.h"
#include "dui_manager.h"
#include "dui_stringutil.h"
#include "dui_notifyevent.h"
#include "scroll/dui_scroll.h"
#include "dui_button.h"
#include "aee_debug.h"
DUIWindow::DUIWindow()
{
	mClassID = DUIStringUtil::StrHash("window");
	mFloatDlg = NULL;
	mFloatDlgFocus = NULL;
	mFocus = NULL;
	mCapture = NULL;
	mPenDownCtrl = NULL;
	mTimeoutID = -1;
	mDialogHandler = NULL;
	mDestoryMonitor = NULL ;
	mMoveScroll = NULL;
	mTimeBefore = 0;
	mTimeAfter = 0;
}

DUIWindow::~DUIWindow()
{
	for(AEEList<DUIScroll*>::iterator 
		sit = mScrollHander.begin(); sit != mScrollHander.end(); ++sit)
	{
		delete *sit;
	}
	if(mTimeoutID>=0)
	//	AEE_IShell_CancelTimer(ZMAEE_GetShell(),mTimeoutID);
		ZMAEE_StopPeriodHandler(0,mTimeoutID);
	mTimeoutID = -1;
	
	if(mDialogHandler)
		delete mDialogHandler;
	mDialogHandler = NULL;

	if(mDestoryMonitor)
		delete mDestoryMonitor;
	mDestoryMonitor = NULL ;

	mFloatDlgFocus = NULL;
}

bool DUIWindow::OnKeyEvent(int flag, int ev, int key)
{
	if(mFloatDlg && mFloatDlg->GetClassId() == DUIStringUtil::StrHash("container"))
	{
	
		if(mFloatDlgFocus && mFloatDlgFocus->OnKeyEvent(flag, ev, key))
		{
			GetManager()->PostRepaintMessage();
			return true;
		}

		if( key == ZMAEE_KEY_LEFT || key == ZMAEE_KEY_RIGHT ||
			key == ZMAEE_KEY_UP || key == ZMAEE_KEY_DOWN)
		{
			if(ev == ZMAEE_EV_KEY_PRESS)
			{
				DUIControl* NextFocus = ((DUIContainer*)mFloatDlg)->GetNextFocusCtrl(
					(key == ZMAEE_KEY_LEFT || key == ZMAEE_KEY_UP) ? true:false);
				if(NextFocus)
					mFloatDlgFocus = NextFocus ;
				GetManager()->PostRepaintMessage();
			}

			if(mFloatDlgIsModel)
				return true ;
		}
	}

	if(GetManager()->IsSupportKey())
	{
		
		if(mFocus && mFocus->OnKeyEvent(flag, ev, key))
			return true;
		
		if(key == ZMAEE_KEY_LEFT || key == ZMAEE_KEY_RIGHT)
		{
			if(ev == ZMAEE_EV_KEY_PRESS)
				SetHNextFocus(key == ZMAEE_KEY_LEFT ? true:false);
			return true;
		}
		else if (key == ZMAEE_KEY_UP ||key == ZMAEE_KEY_DOWN)
		{
		
			if(this->GetID() == DUIStringUtil::StrHash("Bind_scene")){
				return false;
			}
			if(ev == ZMAEE_EV_KEY_PRESS)
				SetVNextFocus(key == ZMAEE_KEY_UP ? true:false);
			return true;
		}
	}
	return false ;
}

bool DUIWindow::OnMotionEvent(const DUIMotionEvent* pMotionEv)
{
	if(mFloatDlg)
	{
		bool ret = mFloatDlg->OnMotionEvent(pMotionEv);
		if(mFloatDlgIsModel)
			return ret ;
	}
	if(mCapture && mCapture->OnMotionEvent(pMotionEv))
	{
		return true;
	}


	if(pMotionEv->GetAction() == DUIMotionEvent::ACTION_DOWN)
	{
		SetFocus(NULL);
	}
	if(DUIContainer::OnMotionEvent(pMotionEv))
	{
// 		DUIControl* pCtrl = GetManager()->GetNotifyedCtrl();
// 		SetFocus(pCtrl);
// 		ReleaseCapture();
		return true;
	}


	if(pMotionEv->GetAction() == DUIMotionEvent::ACTION_UP)
	{
		//PRINTF("Scroll PenUp\n");
	}
	if(mMoveScroll && pMotionEv->GetAction() == DUIMotionEvent::ACTION_MOVE)
	{
		//只处理当前动作的移动
		mMoveScroll->OnMotionEvent(pMotionEv);
		return true;
	}
	else if(mMoveScroll && pMotionEv->GetAction() == DUIMotionEvent::ACTION_UP)
	{
		//只处理当前动作的UP
		mMoveScroll->OnMotionEvent(pMotionEv);
		mMoveScroll = NULL;
		return true;
	}

	AEEList<DUIScroll*>::iterator it;
	for(it = mScrollHander.begin(); it != mScrollHander.end(); ++it)
	{
		if(pMotionEv->GetAction() == DUIMotionEvent::ACTION_DOWN)
		{
			(*it)->OnMotionEvent(pMotionEv);
		}
		else if(pMotionEv->GetAction() == DUIMotionEvent::ACTION_MOVE)
		{
			//某动作的触笔移动判断出当前属于某个scroll的动作
			if((*it)->OnMotionEvent(pMotionEv))
			{
				mMoveScroll = (DUIScroll*)(*it);
				return true;
			}
		}
	}

	return false;
}

DUIControl* DUIWindow::GetCurDialog(bool*isModel)
{
	if(isModel)
		*isModel = mFloatDlgIsModel;
	return mFloatDlg;
}

void DUIWindow::SetDialog(DUIControl *pControl, bool bModel)
{
	DestroyDialog();

	mFloatDlg = pControl;
	Add(pControl);
	mFloatDlgIsModel = bModel;
	RefreshLayout();
}
DUIControl* DUIWindow::ShowDialog(const char* strDialogXml,bool isMOdel ,int nTimeout,AEEDelegateBase*pHandler)
{
	DUIControl* pControl;
	DestroyDialog();
	pControl = mManager->CreateControl(strDialogXml);
	if(pControl)
	{
		mFloatDlg = pControl;
		if(mFloatDlg->GetClassId() == DUIStringUtil::StrHash("container"))
			mFloatDlgFocus = ((DUIContainer*)mFloatDlg)->GetNextFocusCtrl(false);
		Add(pControl);
		mFloatDlgIsModel = isMOdel;
		RefreshLayout();
	}
	/** if
	* @brief: 
	* @Detailed: 
	* @param[in] nTimeout>0: 
	* @pre: 
	* @return:  
	*/
	if(nTimeout>0)
	{
		AEE_IShell* pShell = ZMAEE_GetShell();
		mTimeBefore = AEE_IShell_GetTickCount(pShell);
		if(mTimeoutID >=0)
			//AEE_IShell_CancelTimer(pShell,mTimeoutID);
			ZMAEE_StopPeriodHandler(0,mTimeoutID);
		mTimeoutID = ZMAEE_StartPeriodHandler(0,(void*)this,DUIWindow::_DUITimerCallback);
// 		mTimeoutID = AEE_IShell_SetTimer(
// 			pShell,
// 			nTimeout,
//			DialogTimeoutCb,
// 			AEE_IShell_ActiveApplet(pShell),
// 			this);
	}
	
	mDialogHandler = pHandler ;
		
	return pControl;
}
int DUIWindow::_DUITimerCallback(int /*timerid*/, void* pUser)
{
	DUIWindow* pThis = (DUIWindow*)pUser;
	pThis->mTimeAfter = AEE_IShell_GetTickCount(ZMAEE_GetShell());
	if((pThis->mTimeAfter-pThis->mTimeBefore)>=3000){
		pThis->mTimeBefore = 0;
		pThis->mTimeAfter = 0;
		pThis->DialogTimeoutCb(pThis->mTimeoutID,pThis);
	}
	else{
		ZMAEE_StopPeriodHandler(0,pThis->mTimeoutID);
		pThis->mTimeoutID = -1;
		pThis->mTimeoutID = ZMAEE_StartPeriodHandler(0,(void*)pThis,DUIWindow::_DUITimerCallback);
	}
	return 0;
}

void DUIWindow::DestroyDialog()
{
	mFloatDlgIsModel = true;
	if(mTimeoutID>=0)
		//AEE_IShell_CancelTimer(ZMAEE_GetShell(),mTimeoutID);
		ZMAEE_StopPeriodHandler(0,mTimeoutID);
	mTimeoutID = -1;

	if(mFloatDlg)
	{

		if(mDestoryMonitor)
		{
			mDestoryMonitor->Invoke(this);
			delete mDestoryMonitor;
			mDestoryMonitor = NULL ;
		}

		if(mDialogHandler)
			delete mDialogHandler;
		mDialogHandler = NULL;
		
		Remove(mFloatDlg, true);
		mFloatDlg = NULL;
		mFloatDlgFocus = NULL;
		RefreshLayout();
	}
}
//zhd
void DUIWindow::SetCapture(DUIControl* pCtrl)
{
	mCapture = pCtrl;
	if(mPenDownCtrl)
		mPenDownCtrl->ModifyStyle(0, UISTYLE_PUSHED);
	mPenDownCtrl = NULL;
}

void DUIWindow::ReleaseCapture(void)
{
	mCapture = NULL;
}

void DUIWindow::SetFocus(DUIControl* pCtrl)
{
	if( mFocus != pCtrl) 
	{
		if(pCtrl)
		{
			if(mFocus)
				mFocus->ModifyStyle(0, UISTYLE_FOCUSED);

			pCtrl->ModifyStyle(UISTYLE_FOCUSED,0);
			mFocus = pCtrl;
			if(DUIContainer::SetFocusCtrl(mFocus) == false)
				ResetScroll();
		}
	}	
}

void DUIWindow::RegisterScrollHandler(DUIControl* pControl, DUIScroll* pScroll)
{
	pScroll->SetControl(this, pControl);
	mScrollHander.push_back(pScroll);
}
	
void DUIWindow::UnRegisterScrollHandler(DUIScroll* pScroll)
{
	AEEList<DUIScroll*>::iterator iter;
	iter = mScrollHander.find(pScroll);
	if (iter != NULL) {
		if(pScroll == mMoveScroll)
			mMoveScroll = NULL ;

		pScroll->SetControl(this, NULL);
		delete (*iter);
		mScrollHander.erase(iter);
	}
}


void DUIWindow::DialogTimeoutCb(int timerID,void* pUser)
{
	int bDestorySelf = true ;
	DUIWindow* pWin = (DUIWindow*)pUser;
	if(pWin)
	{
		if(pWin->mTimeoutID>=0)
			//AEE_IShell_CancelTimer(ZMAEE_GetShell(),pWin->mTimeoutID);
			ZMAEE_StopPeriodHandler(0,timerID);
		pWin->mTimeoutID = -1;
		
		if(pWin->mDialogHandler)
			bDestorySelf = pWin->mDialogHandler->Invoke((void*)&pWin);

		if(bDestorySelf)
		{
			pWin->DestroyDialog();
			pWin->GetManager()->PostRepaintMessage();
		}
	}
}



void DUIWindow::SetVNextFocus(bool next,bool redraw)
{
	SetFocus(GetNextFocusCtrl(next));
	
	GetManager()->PostRepaintMessage();
}
void DUIWindow::SetHNextFocus(bool next,bool redraw)
{



}
void DUIWindow::LostKeyFocus()
{
	mFocus = NULL;
}


void DUIWindow::ResetScroll()
{
	if(mFocus == NULL)
		return ;

	TRect FocusRect =  mFocus->GetAbsoluteRect();

	AEEList<DUIScroll*>::iterator it;
	for(it = mScrollHander.begin(); it != mScrollHander.end(); ++it)
	{
		DUIContainer* pContainer = (DUIContainer*)(*it)->GetControl();
		if(pContainer)
		{
			TRect ScrollRect =  pContainer->GetAbsoluteRect();
			if(ScrollRect.y > FocusRect.y)
			{
				DUIScrollInfo sInfo ;
				pContainer->GetScrollInfo(sInfo);
				pContainer->SetScrollPos(sInfo.pos_x,0);
			}
		}
	}
}
void DUIWindow::SetDialogDestorypMonitor(AEEDelegateBase*pMonitor )
{
	mDestoryMonitor = pMonitor ;

}
