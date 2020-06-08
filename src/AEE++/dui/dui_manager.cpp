#include "AEEFileFind.h"
#include "AEEFileMgr.h"
#include "dui_stringutil.h"
#include "dui_manager.h"
#include "dui_edit.h"
#include "dui_notifyevent.h"
#include "aee_debug.h"
#include "Application.h"
#ifdef WIN32
#include <stdio.h>
#pragma warning(disable:4291)
#endif
#define DUI_RES_STRTAB_HEADER	"zmstrtab"
#define DUI_RES_IMGTAB_HEADER	"zmimgtab"
#define DUI_RES_BGTAB_HEADER	"zmbgtab "
#define DUI_RES_BDTAB_HEADER	"zmbdtab "


#ifdef WIN32
#define EXTERN_LAYER_ID 4
#define WATCH_BOTTOM_HEIGHT 36
#endif

DUIManager::DUIManager()
{
	mFillerTab = NULL;
	mBitmapTab = NULL;
	mStrTab = NULL;
	mBuildCreator = NULL;
	m_pCurWnd = NULL;

	ZMAEEDeviceInfo dif;
	AEE_IShell_GetDeviceInfo(ZMAEE_GetShell(), &dif);
	mScreenWidth = dif.cxScreen;
	mScreenHeight = dif.cyScreen;
	mPreKeyHandler = NULL;
	m_pStackAlloc = NULL;
	mRepaintPeriodId = -1;
	mIsSuspend = false;
	mNotifyedCtrl = NULL;
	mSupportKey = false ;

#if (defined WIN32 && defined CIRCLE_SCREEN) 
	{
		ZMAEE_Rect rc = {0};
		AEE_IDisplay *pDisplay = NULL;

		rc.height = mScreenWidth;
		rc.width = mScreenHeight;
		
		pDisplay = mDC.GetDisplay();
		AEE_IDisplay_CreateLayer(pDisplay, EXTERN_LAYER_ID, &rc, ZMAEE_COLORFORMAT_16);

		AEE_IDisplay_SetActiveLayer(pDisplay, EXTERN_LAYER_ID);
		//设置粉红色为透明色
		AEE_IDisplay_SetTransColor(pDisplay, 1, ZMAEE_GET_RGB(255,0, 255));
		//填充黑色
	//	AEE_IDisplay_FillRect(pDisplay, 0, 0, rc.width, rc.height, 0);
		AEE_IDisplay_FillCircle(pDisplay, rc.width / 2, rc.height / 2, rc.width / 2, ZMAEE_GET_RGB(255, 0, 255));
	//	AEE_IDisplay_FillRect(pDisplay, 0, rc.height - WATCH_BOTTOM_HEIGHT, rc.width, WATCH_BOTTOM_HEIGHT, 0);
		
		AEE_IDisplay_SetActiveLayer(pDisplay, 0);
	}
#endif
}

DUIManager::~DUIManager()
{
#if (defined WIN32 && defined CIRCLE_SCREEN)
	AEE_IDisplay *pDisplay = mDC.GetDisplay();

	AEE_IDisplay_FreeLayer(pDisplay, EXTERN_LAYER_ID);
#endif

	if(mRepaintPeriodId != -1)
		ZMAEE_StopPeriodHandler(0, mRepaintPeriodId);

	// 先释放wnd
	if(m_pCurWnd) {
		m_pCurWnd->OnDestroy();
		delete m_pCurWnd;
	}
	while(mWndStack.size() > 0)
	{
		DUIBaseWnd* pWnd = mWndStack.front();
		pWnd->OnDestroy();
		delete pWnd;
		mWndStack.pop_front();
	}
	
	if(mFillerTab) delete mFillerTab;
	if(mBitmapTab) delete mBitmapTab;
	if(mStrTab) delete mStrTab;

	if(mPreKeyHandler)
		delete mPreKeyHandler;
	if(m_pStackAlloc)
		delete m_pStackAlloc;
	if(mBuildCreator)
		delete mBuildCreator;
	mBuildCreator = NULL;
}

void  DUIManager::InitStackAlloc(int nStackSize, int nMaxEntry /*= 32*/)
{
	if(m_pStackAlloc == NULL){
		m_pStackAlloc = new AEEStackAlloc(nStackSize, nMaxEntry);
	}
}

void* DUIManager::StackMalloc(int nSize)
{
	if(m_pStackAlloc)
		return m_pStackAlloc->Alloc(nSize);
	return AEEAlloc::Alloc(nSize);
}

void  DUIManager::StackFree(void* p)
{
	if(m_pStackAlloc)
		m_pStackAlloc->Free(p);
	else
		AEEAlloc::Free(p);
}

void DUIManager::SetExternalBuilder(IBuilderCallback* pBuilder)
{
	mBuildCreator = pBuilder;
}

void DUIManager::SetPreKeyHandler(AEEDelegateBase* keyHandler)
{
	if(mPreKeyHandler)
		delete mPreKeyHandler;
	mPreKeyHandler = keyHandler;
}

bool DUIManager::Suspend(void* param)
{
	SuspendAllBitmap();
	mDC.Suspend(param);
	mIsSuspend = true;
	return false;
}

bool DUIManager::Resume(void* param)
{
	ResumeAllBitmap();
	mDC.Resume(param);
	mIsSuspend = false;
	return false;
}

void DUIManager::OnKeyEvent(int flag, int ev, int key)
{
	Application* papp = GetAppInstance();
	if(papp->IsShowUpdate == true){
		if((key != ZMAEE_KEY_SOFTOK) && (key != ZMAEE_KEY_SOFTRIGHT)){
			return;
		}
	}
	bool isModelDlg = true;

#ifdef WIN32//测试用模拟器左键来模拟安卓回退键
	bool isADKey = (key == ZMAEE_KEY_SOFTLEFT) ? true : false;
#else
	bool isADKey = (key == ZMAEE_KEY_DESKTOP_BACK) ? true : false;
#endif

	if((mPreKeyHandler && m_pCurWnd && 
		(m_pCurWnd->GetInnerWnd()->GetCurDialog(&isModelDlg) == NULL || isModelDlg == false)) ||
		isADKey)
	{
		DUINotifyEvent nv;
		nv.mOwnerWnd = m_pCurWnd->GetInnerWnd();
		nv.mSender = m_pCurWnd->GetInnerWnd();
		nv.mType = DUI_NOTIFY_KEYBORD;
		nv.wParam = ev;
		nv.lParam = key;
		if(mPreKeyHandler->Invoke((void*)&nv))
			return;
	}
	if(m_pCurWnd)
		m_pCurWnd->GetInnerWnd()->OnKeyEvent(flag, ev, key);

}

//触屏事件zm_zhd
void DUIManager::OnMotionEvent(const DUIMotionEvent* pMotionEv)
{
	if(m_pCurWnd)
		m_pCurWnd->GetInnerWnd()->OnMotionEvent(pMotionEv);
}

void DUIManager::SetResDir(const char* resDir)
{
	zmaee_strcpy(mResDir, resDir);
	if(mResDir[zmaee_strlen(mResDir)-1] != '\\')
		zmaee_strcat(mResDir, "\\");
}

const char* DUIManager::GetResDir() const
{
	return mResDir;
}

void DUIManager::LoadResource(const char* resFile, int bUseScreenMem)
{
	AEEFile file;
	{
		char szFileTmp[128] ={0};
		zmaee_strcpy(szFileTmp, mResDir);
		zmaee_strcat(szFileTmp, resFile); 
		if( !file.Open(szFileTmp, AEEFile::modeRead))
			return;
	}

	char szFlag[8];
	while(sizeof(szFlag) == file.Read(szFlag, sizeof(szFlag)))
	{
		//KString temp=NULL;
		if(zmaee_memcmp(szFlag, (void*)DUI_RES_STRTAB_HEADER, sizeof(szFlag)) == 0)
		{
			if(mStrTab == NULL)
				mStrTab = new DUIStringTable();
			mStrTab->Load(&file);
			//temp = mStrTab->GetString(10000);
		}
		else if(zmaee_memcmp(szFlag, (void*)DUI_RES_IMGTAB_HEADER, sizeof(szFlag)) == 0)
		{
			if(mBitmapTab == NULL)
				mBitmapTab = new DUIBitmapTable(this, bUseScreenMem);
			mBitmapTab->Load(&file);
		}
		else if(zmaee_memcmp(szFlag, (void*)DUI_RES_BGTAB_HEADER, sizeof(szFlag)) == 0)
		{
			if(mFillerTab == NULL)
				mFillerTab = new DUIFillerTable(this);
			mFillerTab->LoadBgFiller(&file);
		}
		else if(zmaee_memcmp(szFlag, (void*)DUI_RES_BDTAB_HEADER, sizeof(szFlag)) == 0)
		{
			if(mFillerTab == NULL)
				mFillerTab = new DUIFillerTable(this);
			mFillerTab->LoadBorderFiller(&file);
		}
	}
}

DUIBorderFiller* DUIManager::GetBorderFiller(int nID)
{
	if(mFillerTab == NULL)
		return NULL;
	return mFillerTab->GetBorderFiller(nID);
}

DUIBGFiller* DUIManager::GetBGFiller(int nID)
{
	if(mFillerTab == NULL)
		return NULL;
	return mFillerTab->GetBgFiller(nID);
}

DUIBmpHandle DUIManager::GetBmpHandle( int nID )
{
 	if(mBitmapTab == NULL)
 		return DUIBmpHandle();
 	return mBitmapTab->GetBitmapHandle(nID);	
//	return LoadBmpHandle(nID);
}

KString DUIManager::GetString(int nID)
{
	if(mStrTab == NULL)
		return NULL;
	return mStrTab->GetString(nID);
}
DUIBmpHandle DUIManager::LoadBmpHandle( int nID )
{
	if(mBitmapTab == NULL){
		return DUIBmpHandle();
	}
	return mBitmapTab->LoadBitmapHandle(nID);
}

void DUIManager::UnLoadBitmapHandle( DUIBmpHandle bmpHandle )
{
	if(mBitmapTab)
		mBitmapTab->UnLoadBitmapHandle(bmpHandle);
}

AEE_IImage* DUIManager::LoadImage(const AEEStringRef& imgPath)
{
	char szFileTmp[128] ={0};
	if(imgPath[1] != ':')
		zmaee_strcpy(szFileTmp, mResDir);
	zmaee_memcpy(szFileTmp+zmaee_strlen(szFileTmp), imgPath.ptr(), imgPath.length());

	AEEFileMgr fmgr;
	if(!fmgr.Test(szFileTmp))
		return NULL;

	AEE_IImage* pImg = NULL;
	AEE_IDisplay_CreateImage((AEE_IDisplay*)mDC, AEEAlloc::Alloc, AEEAlloc::Free, (void**)&pImg);
	if(pImg) 
	{
		int ret = AEE_IImage_SetData(pImg, 0, (const unsigned char*)szFileTmp, zmaee_strlen(szFileTmp));
	
		if( ret != E_ZM_AEE_SUCCESS)
		{
			AEE_IImage_Release(pImg);
			pImg = NULL;
		}
	}
	return pImg;
}


//zhd 相当于getbmp
AEE_IBitmap* DUIManager::LoadBitmap(const AEEStringRef& imgPath)
{
	char szFileTmp[128] ={0};
	if(imgPath[1] != ':')
		zmaee_strcpy(szFileTmp, mResDir);
	zmaee_memcpy(szFileTmp+zmaee_strlen(szFileTmp), imgPath.ptr(), imgPath.length());
	AEE_IBitmap* pBitmap = NULL ;
	AEEFileMgr fmgr;
	if(!fmgr.Test(szFileTmp))
		return NULL;
	
	if(AEE_IDisplay_LoadBitmap(mDC,(const char*)szFileTmp,
		AEEAlloc::Alloc,AEEAlloc::Free,(void**)&pBitmap)!= E_ZM_AEE_SUCCESS)
		pBitmap = NULL ;

	return pBitmap ;
}

char* DUIManager::LoadFile(const AEEStringRef& filePath)
{
	if (filePath.length() <= 0) {
		return NULL;
	}
	char szFileTmp[128] = {0};
	zmaee_strcpy(szFileTmp, mResDir);
	zmaee_memcpy(szFileTmp+zmaee_strlen(szFileTmp), filePath.ptr(), filePath.length());
	
	AEEFile file;

	if(file.Open(szFileTmp, AEEFile::modeRead))
	{
		int nFileSize = file.Size();
		char* buf = (char*)StackMalloc(file.Size()+1);
		if(buf) {
			file.Read(buf, nFileSize);
			buf[nFileSize] = 0;
		}
		file.Close();
		return buf;
	}

	return NULL;
}

DUIControl* DUIManager::CreateControl(const char* strContent)
{
	DUIControl* pWnd = NULL;
	DUIBuilder* pBuilder = NEW_OBJECT(StackMalloc(sizeof(DUIBuilder))) DUIBuilder();

	if(pBuilder == NULL)
		return NULL;
	pWnd = pBuilder->Create(strContent, this, mBuildCreator);
	DEL_OBJECT(DUIBuilder, pBuilder, StackFree);

	return pWnd;
}

DUIControl* DUIManager::CreateControlByFile(const char* strFile)
{
	char* szContent = LoadFile(strFile);
	if(szContent == NULL)
		return NULL;
	DUIControl* pWnd = 	CreateControl(szContent);	
	StackFree(szContent);
	return pWnd;
}

//获取当前窗口 zhd
DUIBaseWnd* DUIManager::GetTopWindow()
{
	return m_pCurWnd;
}

//获取指定窗口 zhd
DUIBaseWnd*	DUIManager::GetWindow(const char* strName)
{
	if(!strName)
		return NULL;
	if(m_pCurWnd  && m_pCurWnd->GetInnerWnd()->GetID() == DUIStringUtil::StrHash(strName))
		return m_pCurWnd;

	AEEList<DUIBaseWnd*>::iterator it ;
	for(it = mWndStack.begin();it!= mWndStack.end();it++)
	{
		if(((DUIBaseWnd*)(*it))->GetInnerWnd()->GetID() == DUIStringUtil::StrHash(strName))
			return (DUIBaseWnd*)(*it);
	}

	return NULL;

}

//显示指定窗口，相当于跳转到某一个场景 zhd
//bPushCurWnd		是否需要保留上一个场景
void DUIManager::ShowWindow(DUIBaseWnd* pWnd, bool bPushCurWnd ,bool bRefresh )
{

	
	if(pWnd == NULL)
		return;
	
	pWnd->OnCreate();

	if(pWnd->GetInnerWnd() == NULL)
		return;

	pWnd->GetInnerWnd()->SetRect(0, 0, mScreenWidth, mScreenHeight);

	if( m_pCurWnd != NULL )
	{
		if( bPushCurWnd )
		{
			m_pCurWnd->OnSuspend();
			mWndStack.push_back(m_pCurWnd);
		}
		else
		{
			m_pCurWnd->OnDestroy();
			delete m_pCurWnd;
			m_pCurWnd = NULL;
		}
	}

	m_pCurWnd = pWnd;
	mDC.Reset(true);

	if(bRefresh) {
		Refresh();
	}
	
}

//刷新 zhd
void DUIManager::Refresh()
{

	Application* papp = GetAppInstance();
	if(m_pCurWnd == NULL || mIsSuspend == true)
		return;

	m_pCurWnd->GetInnerWnd()->OnPaint(&mDC, 0, 0);
	mDC.Update(0, 0, mScreenWidth, mScreenHeight);

#if (defined WIN32 && defined CIRCLE_SCREEN)
	{
		AEE_IDisplay *pDisplay = NULL;
		
		pDisplay = mDC.GetDisplay();
		if(pDisplay)
		{
			int flattenLayer[4] = {0};
			int activityLayerId = AEE_IDisplay_GetActiveLayer(pDisplay);
				
			flattenLayer[0] = activityLayerId;
			flattenLayer[1] = EXTERN_LAYER_ID;
			
			AEE_IDisplay_Flatten(pDisplay, 2, flattenLayer, 0);
			AEE_IDisplay_Update(pDisplay, 0, 0, mScreenWidth, mScreenHeight);
		}
	}
#endif
}
//返回上一层 zhd
void DUIManager::Back(bool bToFirst/*=false*/,bool bRefresh/*=false*/)
{
	if(m_pCurWnd == NULL || mWndStack.size() == 0)
		return;

	if(m_pCurWnd) {
		m_pCurWnd->OnDestroy();
		delete m_pCurWnd;
	}
	m_pCurWnd = NULL;

	if(!bToFirst)
	{
		m_pCurWnd = mWndStack.back();
		mWndStack.pop_back();
	}
	else
	{
		m_pCurWnd = mWndStack.front();
		mWndStack.pop_front();

		AEEList<DUIBaseWnd*>::iterator it;
		for( it = mWndStack.begin(); it != mWndStack.end(); ++it)
		{
			(*it)->OnDestroy();
			delete (*it);
		}
		mWndStack.clear();
	}

	if(m_pCurWnd)
		m_pCurWnd->OnResume();

	mDC.Reset(true);
	
	if(bRefresh) Refresh();

}

int DUIManager::_RepaintMessageHandler(int /*nHandle*/, void* pUser)
{
	DUIManager* pThis = (DUIManager*)pUser;
	if(pThis->mRepaintPeriodId != -1)
		ZMAEE_StopPeriodHandler(0,pThis->mRepaintPeriodId);
	pThis->mRepaintPeriodId = -1;
	
	pThis->Refresh();
	return 0;
}

void DUIManager::PostRepaintMessage()
{
	if(mRepaintPeriodId != -1)
		return;
	mRepaintPeriodId = ZMAEE_StartPeriodHandler(0, (void*)this, DUIManager::_RepaintMessageHandler);
}

void DUIManager::SetPtDownControl(DUIControl* pControl)
{
	//PRINTF("SetPtDownControl::%d\n",pControl);
	DUIWindow* pWin = GetTopWindow()->GetInnerWnd();
	pWin->SetPenDownControl(pControl);
}

DUIControl* DUIManager::GetPtDownControl()
{
	DUIWindow* pWin = GetTopWindow()->GetInnerWnd();
//	PRINTF("GetPtDownControl::%d\n",pWin->GetPenDownControl());
	return pWin->GetPenDownControl();
}

void DUIManager::NotifyEvent(DUINotifyEvent* pEv)
{
	DUIBaseWnd *pWnd = GetTopWindow();
	pEv->mOwnerWnd = pWnd->GetInnerWnd();
	if(pEv->mSender->GetClassId() == DUIStringUtil::StrHash("radio"))
	{
		DUIContainer* pContainer = (DUIContainer*)pEv->mSender->GetParent();
		DUIControls& childs = pContainer->GetItems();
		for(DUIControls::iterator it = childs.begin(); it != childs.end(); ++it)
		{
			if((*it)->GetClassId() == pEv->mSender->GetClassId())
			{
				if(*it == pEv->mSender)
					(*it)->ModifyStyle(UISTYLE_CHECKED, 0);
				else
					(*it)->ModifyStyle(0, UISTYLE_CHECKED);
			}
		}
	}
	
	if(pEv->mSender->GetClassId() == DUIStringUtil::StrHash("edit"))
	{
		if(pEv->mType == DUI_NOTIFY_CLICK)
		{
			((DUIEdit*)pEv->mSender)->OnInputClick();
			return;
		}
		else
		{
			if(pEv->wParam == ZMAEE_EV_KEY_RELEASE &&
				pEv->lParam == ZMAEE_KEY_SOFTOK)
			{
				((DUIEdit*)pEv->mSender)->OnInputClick();
				return ;
			}
		}
	}

	pWnd->OnNotifyEvent(pEv);
}




