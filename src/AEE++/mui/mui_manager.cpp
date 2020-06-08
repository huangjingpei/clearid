#include "AEEFile.h"
#include "gdi/AEEBitmapScale.h"
#include "gdi/AEERotateBitmap.h"
#include "mui_manager.h"
#include "mui_tab.h"
#include "mui_builder.h"
#include "mui_window.h"
#include "mui_transform.h"

MUIManager::MUIManager(MUIBmpFactory* pFactory,
					   int nScreenW, int nScreenH, 
					   int nWindowW, int nWindowH, bool isGL)
{
	mTransform = NULL;
	mPeriodTransform = -1;
	mPeroidid = -1;
	mPeriodRedraw = -1;
	mDialog = NULL;
	mTip = NULL;
	mBmpFactory = pFactory;

	
	mBmpFactory->setMgr(this);
	mBgLayerIdx = -1;
	mBgImage = NULL;

	mDC = new MUIDC(isGL);
	mScreenW = nScreenW;
	mScreenH = nScreenH;
	mResWinW = mWindowW = nWindowW;
	mResWinH = mWindowH = nWindowH;
	mFileCache = NULL;
	mFileCacheSize = 0;
	mPaused = 0;


	setGetResHandler(defaultGetResHandler, (void*)this);

	ZMAEE_LayerInfo li;
	mDC->GetLayerInfo(0, &li);
	mColorFormatBaseLayer = li.cf;

	if(mScreenH == mWindowW)
	{
		ZMAEE_Rect rc;
		rc.x = rc.y = 0;
		rc.width = mWindowW;
		rc.height = mWindowH;
		mWorkLayer = mDC->CreateLayer(&rc, mColorFormatBaseLayer);
	}
	else
		mWorkLayer = 0;
	
	mRectRedrawCount = 0;

	mDC->SetActiveLayer(mWorkLayer);
}

MUIManager::~MUIManager()
{
	
	if(mTip) delete mTip;
	
	if(mDialog) delete mDialog;

	while(mWndStack.size() > 0)
	{
		delete mWndStack.back();
		mWndStack.pop_back();
	}
	if(mBgImage) AEE_IImage_Release(mBgImage);

	if(mDC) delete mDC;
	if(mBmpFactory) delete mBmpFactory;



	AEEMap<unsigned int, void*>::iterator it;
	for(it = mWidgetDrawMap.begin(); it != mWidgetDrawMap.end(); ++it)
		delete (MUIWidgetDraw*)it->second;
	mWidgetDrawMap.clear();

	for(it = mTabDrawMap.begin(); it != mTabDrawMap.end(); ++it)
		delete (MUITabDraw*)it->second;
	mTabDrawMap.clear();

	for(it = mScrollDrawMap.begin(); it != mScrollDrawMap.end(); ++it)
		delete (MUIScrollDraw*)it->second;
	mScrollDrawMap.clear();

	stopTransform();
	
	stopAllPeriod();

	
	if(mFileCache) AEEAlloc::Free(mFileCache);
}


int MUIManager::defaultGetResHandler(void* user, const char* name, unsigned char* outData, unsigned int maxSize)
{

	MUIManager* pManager = (MUIManager*)user;
	
	AEEFile file;
	char lpszFile[64];
	zmaee_sprintf(lpszFile, "%s%s", pManager->mUIDir, name);
	if(!file.Open(lpszFile, AEEFile::modeRead))
		return -1;
	int nFileSize = file.Size();
	file.Read(outData, nFileSize);
	outData[nFileSize] = 0;
	file.Close();

	return nFileSize;

}



// 设置文件资源获取数据的方法
void MUIManager::setGetResHandler(funcGetRes handler, void* user)
{
	mFuncGetRes = handler;
	mFuncGetResUser = user;
}

void MUIManager::setResourceScale(int nResWindowW, int nResWindowH)
{
	mResWinH = nResWindowH;
	mResWinW = nResWindowW;
}

void MUIManager::scale(MUIRect& rc)
{
	rc.x = (rc.x * mWindowW) /mResWinW;
	rc.y = (rc.y * mWindowH) / mResWinH;
	rc.width = (rc.width*mWindowW) / mResWinW;
	rc.height = (rc.height * mWindowH) / mResWinH;
}

int MUIManager::scale(int val, int type)
{
	if (type == 1) {
		return (val * mWindowW) / mResWinW;
	} else if (type == 2) {
		return (val* mWindowH) /mResWinH;
	}
	
	return val;
}

void MUIManager::scale(MUIRect& rc, int type)
{
	if (type == 1) {
		rc.x = (rc.x * mWindowW) /mResWinW;
		rc.y = (rc.y * mWindowW) / mResWinW;
		rc.width = (rc.width*mWindowW) / mResWinW;
		rc.height = (rc.height * mWindowW) / mResWinW;
	} else if (type == 2) {
		rc.x = (rc.x * mWindowH) /mResWinH;
		rc.y = (rc.y * mWindowH) / mResWinH;
		rc.width = (rc.width*mWindowH) / mResWinH;
		rc.height = (rc.height * mWindowH) / mResWinH;
	} else if (type == 3) {
		scale(rc);
	}
}

float MUIManager::scalefactor(int type)
{
	if(type == 1)
		return ((float)mWindowW)/mResWinW;
	return ((float)mWindowH)/mResWinH;
}

void MUIManager::onSuspend()
{

	if (mDialog)
		mDialog->OnEvent(MUI_EV_ON_SUSPEND, mDialog, 0, 0);

	MUIWindow* pWnd = GetTopWindow();
	if (pWnd)
		pWnd->OnEvent(MUI_EV_ON_SUSPEND, pWnd, 0, 0);
	
	mDC->Suspend(NULL);

	stopTransform();

	// 放最面，避免之前的MUI_EV_ON_SUSPEND处理中调用了启动Period的函数
	stopAllPeriod();

}

void MUIManager::onResume()
{

	mDC->Resume(NULL);
	
	if(mBgLayerIdx > 0 && mBgImage != NULL)
		restoreBgImage();
	mDC->SetActiveLayer(mWorkLayer);

	if (mDialog)
		mDialog->OnEvent(MUI_EV_ON_RESUME, mDialog, 0, 0);

	MUIWindow* pWnd = GetTopWindow();
	if (pWnd)
		pWnd->OnEvent(MUI_EV_ON_RESUME, pWnd, 0, 0);

}

void MUIManager::restoreBgImage()
{
	cacheBackImage(mBgImage);
}

void MUIManager::stopAllPeriod()
{
	if(mPeroidid >= 0)
	{
		ZMAEE_StopPeriodHandler(0, mPeroidid);
		mPeroidid = -1;
	}
	if(mPeriodRedraw >= 0)
	{
		ZMAEE_StopPeriodHandler(0, mPeriodRedraw);
		mPeriodRedraw = -1;
	}
	
}

bool MUIManager::cacheBackImage(const char* lpszPic)
{
	AEE_IImage* pImg = NULL;
	AEE_IDisplay_CreateImage(mDC->GetDisplay(), AEEAlloc::Alloc, AEEAlloc::Free, (void**)&pImg);
	AEE_IImage_SetData(pImg, 0, (const unsigned char*)lpszPic,  zmaee_strlen(lpszPic));
	bool ret = cacheBackImage(pImg);
	if (!ret) {
		AEE_IImage_Release(pImg);
	}
	return ret;

}

bool MUIManager::cacheBackImage(AEE_IImage* pImg)
{
	if (mBgImage != pImg) {
		if(mBgImage != NULL) AEE_IImage_Release(mBgImage);
		mBgImage  = pImg;
	}

	// gl模式不需要自己做缓存
	if (mDC->IsGL()) {
		return true;
	}

	if(mBgLayerIdx < 0) 
	{
		ZMAEE_Rect rc;
		rc.x = rc.y = 0;
		rc.width = mWindowW;
		rc.height = mWindowH;
		mBgLayerIdx = mDC->CreateLayer(&rc, mColorFormatBaseLayer);
	}
	
	if(mBgLayerIdx < 0) 
	{
		mBgImage = NULL;
		return false;
	}

	int nActive = mDC->GetActiveLayer();

#if defined(WIN32) || defined (__ANDROID__)
	
	if (mResWinW != mWindowW || mResWinH != mWindowH) {
		AEE_IBitmap* pBitmap = NULL;
		AEE_IImage_Decode(pImg, AEEAlloc::Alloc, AEEAlloc::Free, (void**)&pBitmap, 0);
		if(pBitmap == NULL) {
			mBgImage = NULL;
			return false;
		}
		
		
		ZMAEE_BitmapInfo bifSrc={0};
		AEE_IBitmap_GetInfo(pBitmap, &bifSrc);
		ZMAEE_BitmapInfo bifDst={0};
		mDC->GetLayerBmpInfo(mBgLayerIdx, &bifDst);
	
		// TODO 这个缩放并不支持所有的颜色深度格式，所以可能会导致背景无法显示的问题
		_ScaleBilinear(bifSrc, bifDst);
		
		AEE_IBitmap_Release(pBitmap);

	} else {
		mDC->SetActiveLayer(mBgLayerIdx);
		mDC->DrawImageExt(0, 0, mWindowW, mWindowH, pImg, 0);
	}

#else
	mDC->SetActiveLayer(mBgLayerIdx);
	mDC->DrawImageExt(0, 0, mWindowW, mWindowH, pImg, 0);
#endif
	
	mDC->SetActiveLayer(nActive);
	
	return true;
}

bool MUIManager::drawBackground(int x, int y, int cx, int cy)
{
	// 如果是gl模式的，可以直接拉伸绘制AEE_IImage对象
	// gl模式的AEE_IImage对象中会有缓存AEE_IBitmap的,所以不会影响性能
	if (mDC->IsGL()) {
		if (mBgImage)
			mDC->DrawImageExt(0, 0, mWindowW, mWindowH, mBgImage, 0);
		return true;
	}

	if(mBgLayerIdx < 0)
		return false;
	ZMAEE_BitmapInfo bif;
	if (!mDC->GetLayerBmpInfo(mBgLayerIdx, &bif))
		return false;
	mDC->BitBlt(x, y, &bif, x, y, cx, cy, 0, 0);
	return true;
}

void MUIManager::onResize(int nScreenW, int nScreenH, int nWindowW, int nWindowH)
{
	mScreenW = nScreenW;
	mScreenH = nScreenH;
	mWindowW = nWindowW;
	mWindowH = nWindowH;

	if(mWorkLayer > 0) mDC->FreeLayer(mWorkLayer);
	if(mScreenH == mWindowW)
	{
		ZMAEE_Rect rc;
		rc.x = rc.y = 0;
		rc.width = mWindowW;
		rc.height = mWindowH;
		mWorkLayer = mDC->CreateLayer(&rc, mColorFormatBaseLayer);
	}
	else
		mWorkLayer = 0;
}

MUIBmpFactory* MUIManager::getBmpFactory()
{
	return mBmpFactory;
}

MUIDC* MUIManager::getDC()
{
	return mDC;
}

MUIWidget* MUIManager::getCapture()
{
	MUIWindow* pWnd = GetTopWindow();
	if(pWnd == NULL)
		return NULL;

	return pWnd->getCapture();
}

void MUIManager::setCapture(MUIWidget* pWidget)
{
	MUIWindow* pWnd = GetTopWindow();
	if(pWnd == NULL)
		return;	
	pWnd->setCapture(pWidget);
}

bool MUIManager::callWindowProc(int ev, MUIWidget* pSender, int wparam, int lparam)
{
	MUIWidget* pOwner = pSender;
	while(pOwner) {
		if(pOwner->getParent() == NULL)
			break;
		pOwner = pOwner->getParent();
	}

	if(mDialog != NULL && pOwner == mDialog) {
		mDialog->OnEvent(ev, pSender, wparam, lparam);
		return true;
	}

	if(mDialog != NULL && (pOwner == NULL && (ev >= MUI_EV_ON_PEN_DOWN_PREHANDLE && ev <= MUI_EV_ON_KEY_PREHANDLE)))
	{
		if(mDialog->OnEvent(ev, pSender, wparam, lparam))
			return true;
	}

	MUIWindow* pWnd = GetTopWindow();
	if(pWnd == NULL) return false;	
	return pWnd->OnEvent(ev, pSender, wparam, lparam);
}

void MUIManager::onKeyEv(int ev, int code)
{
	// 切换过程不处理按键事件
	if (mPeriodTransform != -1)
		return;
	
	if(mScreenH == mWindowW)
	{
		switch(code)
		{
		case ZMAEE_KEY_LEFT:
			code = ZMAEE_KEY_DOWN;
			break;
		case ZMAEE_KEY_RIGHT:
			code = ZMAEE_KEY_UP;
			break;
		case ZMAEE_KEY_DOWN:
			code = ZMAEE_KEY_RIGHT;
			break;
		case ZMAEE_KEY_UP:
			code =ZMAEE_KEY_LEFT;
			break;
		}
	}

	if(callWindowProc(MUI_EV_ON_KEY_PREHANDLE, NULL, ev, code))
		return;

	if(mDialog != NULL )
	{
		if(mDialog->onKeyEv(ev, code) 
			|| (mDialog->getStyle()&MUI_STYLE_MODEL) == MUI_STYLE_MODEL)
			return;
	}

	MUIWindow* pWndTop = GetTopWindow();
	if(pWndTop == NULL)
		return;

	pWndTop->onKeyEv(ev,code);
}

void MUIManager::onPenEv(int ev, int x, int y)
{
	// 切换过程不处理触笔事件
	if (mPeriodTransform != -1)
		return;

	if(mScreenH == mWindowW)
	{
		int tmp_y = y;
		y = x;
		x = mScreenH-tmp_y;
	}

	//if(ev == ZMAEE_EV_PEN_UP)
	{
		//if(callWindowProc(MUI_EV_ON_PEN_PREHANDLE, NULL, x, y))
		if(callWindowProc(MUI_EV_ON_PEN_DOWN_PREHANDLE + ev - ZMAEE_EV_PEN_DOWN, NULL, x, y))
			return;
	}

	if(mDialog != NULL )
	{
		int bModel = (mDialog->getStyle()&MUI_STYLE_MODEL);
		if(mDialog->onPenEv(ev, x, y) || bModel)
			return;
	}

	MUIWindow* pWndTop = GetTopWindow();
	if(pWndTop == NULL)
		return;
	pWndTop->onPenEv(ev, x, y);
}

int MUIManager::transformCallback(int timerid, void* user)
{
	MUIManager* pThis = (MUIManager*)user;
	MUITransformBase* pTransform = pThis->mTransform;
	if (pTransform) {
		if (pTransform->isFinished()) {
			pThis->stopTransform();
			pThis->redraw(NULL);
			return 0;
		}
		MUIDC* pDC = pThis->getDC();

		pDC->PushAndSetClipRect(0, 0, pThis->getWndWidth(), pThis->getWndHeight());
		pTransform->onDraw(pThis->getDC());
		pDC->PopAndRestoreClipRect();
		
		pThis->updateInternal(NULL);
	}

	return 1;
}

int MUIManager::redrawback(int periodid, void* user)
{
	MUIManager* pThis = (MUIManager*)user;

	pThis->mPeriodRedraw = -1;

	// 页面在切换过程中不处理普通的redraw, 避免引起闪烁
	if (pThis->mPeriodTransform != -1)
		return 0;

	if (pThis->mRectRedrawCount < 0) {
		pThis->redrawInternal(NULL);
	} else {
		int i;
		for (i = pThis->mRectRedrawCount - 1; i >= 0; --i) {
			pThis->redrawInternal(&pThis->mRectRedraws[i]);
		}
	}

	// 清空重绘区域
	pThis->mRectRedrawCount = 0;
	
	pThis->updateInternal(NULL);

	return 0;
}


void MUIManager::redraw(MUIRect* pRect, bool bDelay)
{

	if (mPeriodTransform != -1)
		return;

	if (getDC()->IsGL()) {
		pRect = NULL;
	}

	if(bDelay) {
		if (pRect == NULL || 
			mRectRedrawCount < 0 || 
			mRectRedrawCount >= sizeof(mRectRedraws) / sizeof(mRectRedraws[0])) {
			mRectRedrawCount = -1;
		} else {
			MUIRect* pRcDst = &mRectRedraws[mRectRedrawCount++];
			*pRcDst = *pRect;
		}
		
		if (mPeriodRedraw < 0) 
			mPeriodRedraw = ZMAEE_StartPeriodHandler(0, this, redrawback);

		return;
	}

	redrawInternal(pRect);

	updateInternal(pRect);
}


void MUIManager::onTopPaint(MUIDC* pDC)
{
	
}

void MUIManager::redrawInternal(MUIRect* pRect)
{
	MUIWindow* pWndTop = GetTopWindow();
	if(pWndTop == NULL)
		return;


	MUIRect rc;
	rc = (pRect != NULL) ? (*pRect) : GetTopWindow()->getRect();
	
	mDC->PushAndSetClipRect(rc.x, rc.y, rc.width, rc.height);
	
	pWndTop->onPaint(mDC);
	
	if(mDialog)
	{
		
//#if defined(__ANDROID__) || defined(WIN32)
//		if (mDialog->getStyle() & MUI_STYLE_MODEL) {
//			MUIRect rcScreen;
//			rcScreen.x = rcScreen.y = 0;
//			rcScreen.width = mWindowW;
//			rcScreen.height = mWindowH;
//			mDC->AlphaBlendRect(&rcScreen, ZMAEE_GET_RGBA(0,0,0,128));
//		}
//#endif //__ANDROID__
		
		//	rc = mDialog->getRect();
		//	mDC->SetClipRect(rc.x, rc.y, rc.width, rc.height);
		mDialog->onPaint(mDC);
		
	}
	
	if(mTip)
	{
		mTip->onPaint(mDC);
	}
	
	onTopPaint(mDC);

#ifdef WIN32
//	ZMAEE_DebugPrint("\nredraw rect=%d,%d,%d,%d\n", rc.x, rc.y, rc.width, rc.height);
//	mDC->DrawRect(rc.x, rc.y, rc.width, rc.height, ZMAEE_GET_RGB(0, 255, 0));
#endif

	
	mDC->PopAndRestoreClipRect();
}

void MUIManager::updateInternal(MUIRect* pRect)
{
	if(mScreenH == mWindowW && mWorkLayer > 0)
	{
		ZMAEE_BitmapInfo bif={0};
		mDC->SetActiveLayer(0);
		mDC->GetLayerBmpInfo(mWorkLayer, &bif);
		
		//mDC->BitBlt(mWindowH-rc.y-rc.width, rc.x, &bif, rc.x, rc.y, rc.width, rc.height, ZMAEE_TRANSFORMAT_ROTATE_90, 0);
		mDC->BitBlt(0, 0, &bif, 0, 0, bif.width, bif.height, ZMAEE_TRANSFORMAT_ROTATE_270, 0);
		mDC->SetActiveLayer(mWorkLayer);
		
	}
	
	int updateLayer[1] = {0};
	MUIRect rcScreen;
	rcScreen.x = rcScreen.y = 0;
	rcScreen.width = mScreenW;
	rcScreen.height = mScreenH;
	
	mDC->UpdateEx(&rcScreen, 1, updateLayer);

}


void MUIManager::startTransform()
{
	if (mTransform == NULL)
		return;
	mTransform->start();
	if(mPeriodTransform != -1) {
		ZMAEE_StopPeriodHandler(0, mPeriodTransform);
	}
	mPeriodTransform = ZMAEE_StartPeriodHandler(0, this, transformCallback);
}

void MUIManager::stopTransform()
{
	if(mPeriodTransform != -1) {
		ZMAEE_StopPeriodHandler(0, mPeriodTransform);
		mPeriodTransform = -1;
	}
	if (mTransform) {
		mTransform->release();
		mTransform = NULL;
	}
}

void MUIManager::setTransform(MUITransformBase* pTransform)
{

	if(!pTransform)
		return;
	if (pTransform != mTransform) {
		stopTransform();
		// 临时保存, 避免函数返回时造成内存泄露, 在调用stopTransform时释放
		mTransform = pTransform;
		if (pTransform) {
			pTransform->addRef();
		}
	}
}

MUIWindow* MUIManager::createWindow(const char* lpszUIFile, MUIHandler* pHandler)
{

	if (pHandler == NULL)
		return NULL;
	MUIBuilder builder(pHandler);

	int size = mFuncGetRes(mFuncGetResUser, lpszUIFile, (unsigned char*)mFileCache, mFileCacheSize);
	if (size <= 0) {
		delete pHandler;
		return NULL;
	}

	MUIWindow* pWnd = (MUIWindow*)builder.createWindow(this, mFileCache);
	
	if (pWnd)
		pWnd->setHandler(pHandler);

	return pWnd;
}

void MUIManager::showWindow(const char* lpszUIFile, MUIHandler* pHandler, MUITransformBase* pTransform, bool bPushCurWnd, bool bDialogMode)
{
	int tmp = mColorFormatBaseLayer;
	if (bDialogMode) {
		mColorFormatBaseLayer = ZMAEE_COLORFORMAT_PM32;
	}

	setTransform(pTransform);

	MUIWindow* wnd = mWndStack.back();
	
	// 设置底层window
	if (pTransform) {
		pTransform->setBackWindow(wnd, mColorFormatBaseLayer);
	}
	
	if (bPushCurWnd == false)
	{
		if (wnd != NULL)
		{
			mWndStack.pop_back();
			delete wnd;
		}
	}

	MUIWindow* pWnd = createWindow(lpszUIFile, pHandler);
	
	if (pWnd)
		showWindowInternal(pWnd, pTransform);
	if (bDialogMode) {
		mColorFormatBaseLayer = tmp;
	}
	if (pTransform)
		pTransform->release();

}

void MUIManager::showDialog(const char* lpszUIFile, MUIHandler* pHandler, MUITransformBase* pTransform)
{

	setTransform(pTransform);

	MUIWindow* pWnd = createWindow(lpszUIFile, pHandler);

	if (pWnd)
		showDialogInternal(pWnd, pTransform);

	if (pTransform)
		pTransform->release();

}

void MUIManager::showWindowInternal(MUIWindow* pWnd, MUITransformBase* pTransform)
{

	callWindowProc(MUI_EV_ON_BACKGROUND, NULL, 0, 0);
	mWndStack.push_back(pWnd);

	MUIWndEvent ev = {0};
	ev.owner = NULL;
	ev.ev = MUI_EV_ON_CREATE;
	ev.sender = pWnd;
	pWnd->getHandler()->onHandleEv(&ev);

	// 设置顶层window
	if (pTransform) {
		pTransform->setTopWindow(pWnd, mColorFormatBaseLayer);
		startTransform();
	} else {
		redraw(NULL, false);
	}

}



void MUIManager::showDialogInternal(MUIWindow* pWnd, MUITransformBase* pTransform)
{
	if(mDialog)
	{
		delete mDialog;
		mDialog = NULL;
	}

	//callWindowProc(MUI_EV_ON_BACKGROUND, NULL, 0, 0);
	
	// 设置底层window
	if (pTransform) {
		pTransform->setBackWindow(mWndStack.back(), mColorFormatBaseLayer);
	}
	
	mDialog = pWnd;
	mDialog->SetNeedBg(false);
	MUIWndEvent ev = {0};
	ev.owner = NULL;
	ev.ev = MUI_EV_ON_CREATE;
	ev.sender = pWnd;
	pWnd->getHandler()->onHandleEv(&ev);

	if (pTransform) {
		pTransform->setTopWindow(pWnd, ZMAEE_COLORFORMAT_PM32);
		startTransform();
	} else {
		redraw(NULL);
	}

}

void MUIManager::showTip(const char* lpszUIFile, MUIHandler* pHandler)
{
	if (pHandler == NULL)
		return;

	if(mTip) {
		delete mTip;
		mTip = NULL;
	}

	MUIBuilder builder(pHandler);

	int size = mFuncGetRes(mFuncGetResUser, lpszUIFile, (unsigned char*)mFileCache, mFileCacheSize);
	if (size <= 0) {
		delete pHandler;
		return;
	}


	mTip = (MUIWindow*)builder.createWindow(this, mFileCache);
	mTip->setHandler(pHandler);

	mTip->SetNeedBg(false);

	MUIRect rc = mTip->getRect();

	MUIWndEvent ev = {0};
	ev.owner = NULL;
	ev.ev = MUI_EV_ON_CREATE;
	ev.sender = mTip;
	mTip->getHandler()->onHandleEv(&ev);
	redraw(&rc, false);
}

void MUIManager::destroyDialog(MUITransformBase* pTransform)
{

	setTransform(pTransform);

	if(mDialog)
	{
		if (pTransform) {
			pTransform->setTopWindow(mDialog, ZMAEE_COLORFORMAT_PM32);
		}
		
		delete mDialog;
		mDialog = NULL;
		
		if (pTransform) {
			pTransform->setBackWindow(mWndStack.back(), mColorFormatBaseLayer);
			startTransform();
		} else {
			//callWindowProc(MUI_EV_ON_FOREGROUND, NULL, 0,0);
			redraw(NULL);
		}

	}

	if (pTransform)
		pTransform->release();
}

void MUIManager::destroyTip()
{
	if(mTip)
	{
		delete mTip;
		mTip = NULL;

		redraw(NULL);
	}
}

MUIWindow* MUIManager::GetTopWindow()
{
	if(mWndStack.size() == 0)
		return NULL;
	return mWndStack.back();
}

int MUIManager::delayback(int peroidid, void* user)
{
	MUIManager* pThis = (MUIManager*)user;
	
	pThis->BackInternal(pThis->mTransform);

	return 0;
}

void MUIManager::BackInternal(MUITransformBase* pTransform)
{

	if (mWndStack.size() <= 0)
		return;

	MUIWindow* wnd = mWndStack.back();
	if(wnd == NULL) return;

	mWndStack.pop_back();

	if (pTransform) {
		pTransform->setTopWindow(wnd, mColorFormatBaseLayer);
	}
	delete wnd;
	mPeroidid = -1;

	callWindowProc(MUI_EV_ON_FOREGROUND, NULL, 0,0);
	
	if (pTransform) {
		pTransform->setBackWindow(mWndStack.back(), mColorFormatBaseLayer);
		startTransform();
	} else {
		redraw(NULL);
	}

		

}

void MUIManager::Back(bool bDelay, MUITransformBase* pTransform)
{
	setTransform(pTransform);

	if (bDelay)
	{
		if(mPeroidid != -1)
		{
			ZMAEE_StopPeriodHandler(0, mPeroidid);
		}
		mPeroidid = ZMAEE_StartPeriodHandler(0, this, delayback);
	} else {
		BackInternal(pTransform);
	}

	if (pTransform)
		pTransform->release();
}

void  MUIManager::regWidgetDrawer(const AEEStringRef& strName, void* pWidgetDraw)
{
	unsigned int hashId = MUIUtil::StrHash(strName.ptr(), strName.length());
	mWidgetDrawMap.insert(hashId, pWidgetDraw);
}

void* MUIManager::getWidgetDrawer(const AEEStringRef& strName)
{
	AEEMap<unsigned int, void*>::iterator it;
	unsigned int hashId = MUIUtil::StrHash(strName.ptr(), strName.length());
	it = mWidgetDrawMap.find(hashId);
	
	if(it != mWidgetDrawMap.end())
		return it->second;

	return NULL;
}

void  MUIManager::regTabDrawer(const AEEStringRef& strName,void* pTabDraw)
{
	unsigned int hashId = MUIUtil::StrHash(strName.ptr(), strName.length());
	mTabDrawMap.insert(hashId,  pTabDraw);
}

void* MUIManager::getTabDrawer(const AEEStringRef& strName)
{
	AEEMap<unsigned int, void*>::iterator it;
	unsigned int hashId = MUIUtil::StrHash(strName.ptr(), strName.length());
	it = mTabDrawMap.find(hashId);
	
	if(it != mTabDrawMap.end())
		return it->second;

	return NULL;
}

void  MUIManager::regScrollDrawer(const AEEStringRef& strName, void* pScrollDraw)
{
	unsigned int hashId = MUIUtil::StrHash(strName.ptr(), strName.length());
	mScrollDrawMap.insert(hashId, pScrollDraw);
}

void* MUIManager::getScrollDrawer(const AEEStringRef& strName)
{
	AEEMap<unsigned int, void*>::iterator it;
	unsigned int hashId = MUIUtil::StrHash(strName.ptr(), strName.length());
	it = mScrollDrawMap.find(hashId);

	if( it != mScrollDrawMap.end())
		return it->second;

	return NULL;
}

void MUIManager::setUIDirInfo(const char* lpszDir, int nMaxFileSize)
{
	zmaee_strcpy(mUIDir, lpszDir);
	if(mUIDir[zmaee_strlen(lpszDir)-1]!='\\')
		zmaee_strcat(mUIDir, "\\");
	mFileCacheSize = nMaxFileSize;
	mFileCache = (char*)AEEAlloc::Alloc(nMaxFileSize);
}



AEE_IBitmap* MUIManager::createWindowBitmap(MUIWindow* pWnd, int width, int height, ZMAEE_ColorFormat cf)
{

	AEE_IBitmap* bmp;
	if (pWnd == NULL)
		return NULL;
	
	if (cf != ZMAEE_COLORFORMAT_16 && cf != ZMAEE_COLORFORMAT_PM32)
		return NULL;

	MUIDC* pDC = getDC();
	if (E_ZM_AEE_SUCCESS != AEE_IDisplay_CreateBitmap(pDC->GetDisplay(),
		width, height, cf, AEEAlloc::Alloc, AEEAlloc::Free, (void**)&bmp))
		return NULL;
	ZMAEE_BitmapInfo bi;
	AEE_IBitmap_GetInfo(bmp, &bi);
	unsigned int size = width * height * 2;
	if (cf == ZMAEE_COLORFORMAT_PM32) {
		size *= 2;
	}
	zmaee_memset(bi.pBits, 0, size);

	ZMAEE_Rect rect = {0, 0, width, height};
	
	

	int layerid = pDC->CreateLayer(&rect, cf, bi.pBits, size);
	if (layerid < 0) {
		AEE_IBitmap_Release(bmp);
		return NULL;
	}

	int oldlayer = pDC->GetActiveLayer();
	pDC->SetActiveLayer(layerid);
	
	pWnd->onPaint(pDC);
	
	pDC->FreeLayer(layerid);
	pDC->SetActiveLayer(oldlayer);

	return bmp;
}

