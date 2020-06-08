#ifndef _MUI_MANAGER_H_
#define _MUI_MANAGER_H_

#include "zmaee_image.h"
#include "util/AEEList.h"
#include "util/AEEStringRef.h"
#include "util/AEEMap.h"
#include "mui_bmpfactory.h"
#include "mui_util.h"

class MUIWidget;
class MUIDC;
class MUIWindow;
class MUIHandler;
class MUITransformBase;

typedef int (*funcGetRes)(void* user, const char* name, unsigned char* outData, unsigned int maxSize);

class MUIManager
{
public:
	enum {
		ScaleDisable = 0, 
		ScaleByWidth = 1,
		ScaleByHeight = 2,
		ScaleByEach = 3
	};
	MUIManager(MUIBmpFactory* pBmpFactory,
			   int nScreenW, int nScreenH, int nWindowW, int nWindowH, bool isGL = false);
	virtual ~MUIManager();

	virtual void onSuspend();
	virtual void onResume();
	virtual bool callWindowProc(int ev, MUIWidget* pSender, int wparam, int lparam);
	virtual void onKeyEv(int ev, int code);
	virtual void onPenEv(int ev, int x, int y);
	virtual void onResize(int nScreenW, int nScreenH, int nWindowW, int nWindowH);

	// 设置文件资源获取数据的方法
	void setGetResHandler(funcGetRes handler, void* user);

	void setResourceScale(int nResWindowW, int nResWindowH);
	int  scale(int val, int type);//@type==1 width scale @type==2 height scale
	void scale(MUIRect& rc);
	float scalefactor(int type);
	void scale(MUIRect& rc, int type);//@type==0 both scale @type==1 width scale @type==2 height scale

	MUIBmpFactory* getBmpFactory();
	MUIDC* getDC();
	MUIWidget* getCapture();
	void setCapture(MUIWidget* pWidget);
	MUIWindow* createWindow(const char* lpszUIFile, MUIHandler* pHandler);
	void showWindow(const char* lpszUIFile, MUIHandler* pHandler, MUITransformBase* pTransform = NULL, bool bPushCurWnd = true, bool bDialogMode = false);
	void showDialog(const char* lpszUIFile, MUIHandler* pHandler, MUITransformBase* pTransform = NULL);
	void showTip(const char* lpszUIFile, MUIHandler* pHandler);
	void Back(bool bDelay = true, MUITransformBase* pTransform = NULL);	
	void destroyDialog(MUITransformBase* pTransform = NULL);
	void destroyTip();
	MUIWindow* getDialog() const { return mDialog; }
	MUIWindow* GetTopWindow();
	void redraw(MUIRect* pRect, bool bDelay=true);

	bool cacheBackImage(const char* lpszPic);

	bool drawBackground(int x, int y, int cx, int cy);
	int  getWndWidth() const{ return mWindowW; }
	int  getWndHeight() const{ return mWindowH; }
	
	void  regWidgetDrawer(const AEEStringRef& strName, void* pWidgetDraw);
	void* getWidgetDrawer(const AEEStringRef& strName);
	void  regTabDrawer(const AEEStringRef& strName,void* pTabDraw);
	void* getTabDrawer(const AEEStringRef& strName);
	void  regScrollDrawer(const AEEStringRef& strName, void* pScrollDraw);
	void* getScrollDrawer(const AEEStringRef& strName);

	void  setUIDirInfo(const char* lpszDir, int nMaxFileSize);

	AEE_IBitmap* createWindowBitmap(MUIWindow* pWnd, int width, int height, ZMAEE_ColorFormat cf);

	void startTransform();
	void stopTransform();
	void setTransform(MUITransformBase* pTransform);



protected:

	void showWindowInternal(MUIWindow* pWnd, MUITransformBase* pTransform);
	void showDialogInternal(MUIWindow* pDialog, MUITransformBase* pTransform);
	void BackInternal(MUITransformBase* pTransform);	
	static int delayback(int peroidid, void* user);
	static int redrawback(int periodid, void* user);
	static int transformCallback(int periodid, void* user);
	static int defaultGetResHandler(void* user, const char* name, unsigned char* outData, unsigned int maxSize);
	
	bool cacheBackImage(AEE_IImage* img);
	void restoreBgImage();
	void stopAllPeriod();
	virtual void onTopPaint(MUIDC* pDC);
	virtual void redrawInternal(MUIRect* pRect);
	virtual void updateInternal(MUIRect* pRect);
	
	
protected:
	MUIDC* mDC;
	int	   mScreenW;
	int    mScreenH;
	int	   mWindowW;
	int	   mWindowH;
	int	   mResWinW;
	int	   mResWinH;
	int	   mPaused;
	MUIBmpFactory* mBmpFactory;
	
	funcGetRes mFuncGetRes;
	void* mFuncGetResUser;

	AEEList<MUIWindow*> mWndStack;
	MUIWindow* mDialog;

	MUIWindow* mTip;
	MUITransformBase* mTransform;
	
	int	   mWorkLayer;

	int    mBgLayerIdx;
	AEE_IImage* mBgImage;

	AEEMap<unsigned int, void*> mWidgetDrawMap;
	AEEMap<unsigned int, void*> mTabDrawMap;
	AEEMap<unsigned int, void*> mScrollDrawMap;
	unsigned int mFileCacheSize;
	char* mFileCache;
	char  mUIDir[64];
	int	  mPeroidid;
	int   mPeriodRedraw;
	int   mPeriodTransform;
	int	  mColorFormatBaseLayer;
	int	  mRectRedrawCount;
	MUIRect mRectRedraws[4];
	
};

#endif//_MUI_MANAGER_H_
