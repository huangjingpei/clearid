#ifndef _DUI_WINDOW_H_
#define _DUI_WINDOW_H_

#include "AEEDelegate.h"
#include "util/AEEList.h"
#include "util/AEEMap.h"
#include "dui_container.h"

#ifdef WIN32
#include <stdio.h>
#endif
class DUIScroll;
class DUIWindow : public DUIContainer
{
public:
	DUIWindow();
	virtual ~DUIWindow();
	virtual bool  OnKeyEvent(int flag, int ev, int key);
	virtual bool  OnMotionEvent(const DUIMotionEvent* pMotionEv);

public:
	DUIControl*	  GetCurDialog(bool*isModel =NULL);
	DUIControl*   ShowDialog(const char* strDialogXml,bool isMOdel ,int nTimeout = 0,	AEEDelegateBase*pHandler = NULL);
	void		  DestroyDialog();
	void		  SetCapture(DUIControl* pCtrl);
	void		  ReleaseCapture();
	void		  LostKeyFocus();
	DUIControl*	  GetCapture(){ return mCapture; }
	void		  SetPenDownControl(DUIControl* pControl){mPenDownCtrl = pControl;}
	DUIControl*	  GetPenDownControl() { return mPenDownCtrl; }
	void		  SetFocus(DUIControl* pCtrl);
	DUIControl*	  GetFocus(){
		return mFocus ;
	};
	void		  RegisterScrollHandler(DUIControl* pControl , DUIScroll* pScroll);
	void		  UnRegisterScrollHandler(DUIScroll* pScroll);

	static void	  DialogTimeoutCb(int timerID,void* pUser);

	void		  SetVNextFocus(bool next,bool redraw = true);
	void		  SetHNextFocus(bool next,bool redraw =true);
	AEEList<DUIScroll*>& GetScrollList(){return mScrollHander;}


	void SetDialogDestorypMonitor(AEEDelegateBase*pMonitor );
	
	//add by lts at 2015-10-20
	void SetDialog(DUIControl *pControl, bool bModel);
	static int _DUITimerCallback(int timerid, void* pUser);

protected:
	void		  ResetScroll();
public:
	AEEDelegateBase*mDialogHandler;
	AEEDelegateBase* mDestoryMonitor;
private:
	DUIControl*   mFloatDlg;
	DUIControl*	  mFloatDlgFocus;
	bool		  mFloatDlgIsModel; //是否模态 模态就是不能操作dialog下面的东西
	DUIControl*   mCapture;
	DUIControl*   mFocus;
	DUIControl*	  mPenDownCtrl;
	AEEList<DUIScroll*>	 mScrollHander;
	int			  mTimeoutID;

	DUIScroll*		mMoveScroll;	//当前做动作的Scroll
	long					mTimeBefore;
	long					mTimeAfter;
};

#endif//_DUI_WINDOW_H_
