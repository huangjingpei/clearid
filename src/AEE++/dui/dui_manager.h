#ifndef _DUI_MANAGER_H_
#define _DUI_MANAGER_H_

#include "AEEDelegate.h"
#include "util/AEEList.h"
#include "AEEStackAlloc.h"
#include "dui_dc.h"
#include "dui_bdfiller.h"
#include "dui_bgfiller.h"
#include "dui_stringtable.h"
#include "dui_fillertable.h"
#include "dui_bitmaptable.h"
#include "dui_builder.h"
#include "dui_motionevent.h"
#include "dui_basewnd.h"

#ifdef __DUI_CONTROL_FOR_DESIGNER__
#define NEW_OBJECT(p) new
#define DEL_OBJECT(klass, p, pfree) delete p
#else
#define NEW_OBJECT(p) new(p)
#define DEL_OBJECT(klass, p, pfree) do{p->~klass(); pfree(p); p = 0;}while(0)
#endif

//т╡фа zm_zhd
#ifndef CIRCLE_SCREEN
//#define CIRCLE_SCREEN
#endif

class DUIManager
{
public:
	DUIManager();
	virtual ~DUIManager();
	DUIDC*			GetDC(){ return &mDC; }
	bool			Suspend(void*);
	bool			Resume(void*);
	void			OnKeyEvent(int flag, int ev, int key);
	void			OnMotionEvent(const DUIMotionEvent* pMotionEv);
	void			SetExternalBuilder(IBuilderCallback* pBuilder);
	void			SetPreKeyHandler(AEEDelegateBase* keyHandler);

	void			SetSupportKey(bool bSupport = true) {mSupportKey = bSupport;}
	bool			IsSupportKey() {return mSupportKey ;}
	bool			IsSuspend(){return mIsSuspend;}
	//////////////////////////////////////////////////////////////////////////
	//stack allock
	//////////////////////////////////////////////////////////////////////////
	void			InitStackAlloc(int nStackSize, int nMaxEntry = 32);
	void*			StackMalloc(int nSize);
	void			StackFree(void* p);

	//////////////////////////////////////////////////////////////////////////
	//resource
	//////////////////////////////////////////////////////////////////////////
	void			SetResDir(const char* szResDir);
	void			LoadResource(const char* resFile, int bUseScreenMem = false);
	const char*		GetResDir() const;
	DUIBorderFiller*GetBorderFiller(int nID);
	DUIBGFiller*	GetBGFiller(int nID);
	KString			GetString(int nID);
	DUIBmpHandle	GetBmpHandle(int nID);
	DUIBmpHandle	LoadBmpHandle(int nID);
	void			UnLoadBitmapHandle(DUIBmpHandle bmpHandle);
	AEE_IImage*		LoadImage(const AEEStringRef& imgPath);
	AEE_IBitmap*	LoadBitmap(const AEEStringRef& imgPath);
	char*			LoadFile(const AEEStringRef& filePath);
	void			SuspendAllBitmap(){if(mBitmapTab)mBitmapTab->SuspendAll();}
	void			ResumeAllBitmap(){if(mBitmapTab)mBitmapTab->ResumeAll();}
	DUIFillerTable*	GetFillerTable(){return mFillerTab;}
	DUIBitmapTable* GetTableBmp(){return mBitmapTab;}
	//////////////////////////////////////////////////////////////////////////
	//window
	//////////////////////////////////////////////////////////////////////////
	DUIControl*		CreateControl(const char* strContent);
	DUIControl*		CreateControlByFile(const char* strFile);
	void			ShowWindow(DUIBaseWnd* pWnd, bool bPushCurWnd = true,bool bRefresh = true);
	void			Refresh();
	DUIBaseWnd*		GetTopWindow();
	DUIBaseWnd*		GetWindow(const char* strName);
	void			Back(bool bToFirst = false, bool bRefresh=true);
	void			PostRepaintMessage();
	int				GetHistroyCount() const { return mWndStack.size(); }
	AEEList<DUIBaseWnd*>&	GetHistory() {return mWndStack;}

	//////////////////////////////////////////////////////////////////////////
	//event
	//////////////////////////////////////////////////////////////////////////
	void			SetPtDownControl(DUIControl* pControl);
	DUIControl*		GetPtDownControl();
	void			NotifyEvent(DUINotifyEvent* pEv);
	
	void			SetNotifyedCtrl(DUIControl* pControl) {mNotifyedCtrl = pControl;}
	DUIControl*		GetNotifyedCtrl() {return mNotifyedCtrl;}

	void SetScreenWidth(int width)
	{
		mScreenWidth = width;
	}

	void SetScreenHeight(int Height)
	{
		mScreenHeight = Height;
	}
protected:
	static int		_RepaintMessageHandler(int nHandle, void* pUser);

protected:
	DUIDC					mDC;
	char					mResDir[128];
	int						mScreenWidth;
	int						mScreenHeight;
	AEEStackAlloc*			m_pStackAlloc;
	DUIStringTable*			mStrTab;
	DUIFillerTable*			mFillerTab;
	DUIBitmapTable*			mBitmapTab;
	IBuilderCallback*		mBuildCreator;

	DUIBaseWnd*				m_pCurWnd;
	AEEList<DUIBaseWnd*>	mWndStack;
	AEEDelegateBase*		mPreKeyHandler;
	int						mRepaintPeriodId;
	bool					mIsSuspend;

	DUIControl*				mNotifyedCtrl;

	bool					mSupportKey;
	
};

#endif//_DUI_MANAGER_H_
