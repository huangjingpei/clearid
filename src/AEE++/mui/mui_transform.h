#ifndef _MUI_TRANSFORM_H_
#define _MUI_TRANSFORM_H_

#include "util/AEEStringRef.h"
#include "mui_dc.h"
#include "mui_util.h"

class MUIWindow;
class MUITransform
{
public:
	
	// 设置底层window,处于top window下面
	virtual void setBackWindow(MUIWindow* pWnd, int cf) = 0;
	// 设置顶层window,处于back window上面
	virtual void setTopWindow(MUIWindow* pWnd, int cf) = 0;
	// 调用后开始进行切换
	virtual void start() = 0;
	// 切换窗口是否已经结束
	virtual bool isFinished() = 0;
	// 切换过程的绘制
	virtual void onDraw(MUIDC* pDC) = 0;

};



class MUITransformBase : public MUITransform {
public:

	void addRef() {
		++mRef;
	}
	void release() {
		if (--mRef <= 0)
			delete this;
	}
	
protected:
	MUITransformBase() {
		mRef = 1;
	}

	virtual ~MUITransformBase();

	static unsigned long getTick();
private:
	int		mRef;
};


// class MUITransformBasePtr {
// public:
// 	MUITransformBasePtr(MUITransformBase* ptr) {
// 		mPtr = ptr;
// 	}
// 	~MUITransformBasePtr() {
// 		if (mPtr) mPtr->release();
// 	}
// 	inline MUITransformBase* operator->() { return mPtr; }
// 	inline operator MUITransformBase*() { return mPtr; }
// private:
// 	MUITransformBase*		mPtr;
// };


// 实现window基本的2D变换切换效果
class MUITransform2D : public MUITransformBase
{
public:
	MUITransform2D(int during = 512);
	virtual ~MUITransform2D();
	
	void setDuring(int during);
	void setMoveParam(int startX, int startY, int endX, int endY);
	void setAlphaParam(int startAlpha, int endAlpha);
	void setScaleParam(float startScale, float endScale);
protected:
	
	virtual void setBackWindow(MUIWindow* pWnd, int cf);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
	virtual void start();
	virtual bool isFinished();
	virtual void onDraw(MUIDC* pDC);
	
protected:
	AEE_IBitmap*	mOldBitmap;
	AEE_IBitmap*	mNewBitmap;
	int				mDuring;
	int				mStartX;
	int				mStartY;
	int				mEndX;
	int				mEndY;
	int				mCurrentX;
	int				mCurrentY;

	int				mStartAlpha;
	int				mEndAlpha;
	int				mCurrentAlpha;
	
	float			mStartScale;
	float			mEndScale;
	float			mCurrentScale;

	unsigned long	mTickStart;
	unsigned long	mTickCurrent;
	
};

// 页面从左进去
class MUITransformLeftIn : public MUITransform2D
{
public:
	MUITransformLeftIn(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);

};

// 页面从左出来
class MUITransformLeftOut : public MUITransform2D
{
public:
	MUITransformLeftOut(int during = 512, bool fade = true) ;
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// 页面从右进去
class MUITransformRightIn : public MUITransform2D
{
public:
	MUITransformRightIn(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);

};

// 页面从右出来
class MUITransformRightOut : public MUITransform2D
{
public:
	MUITransformRightOut(int during = 512, bool fade = true) ;
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// 页面从上进去
class MUITransformTopIn : public MUITransform2D
{
public:
	MUITransformTopIn(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// 页面从上出来
class MUITransformTopOut : public MUITransform2D
{
public:
	MUITransformTopOut(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// 页面从下进去
class MUITransformBottomIn : public MUITransform2D
{
public:
	MUITransformBottomIn(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// 页面从下出来
class MUITransformBottomOut : public MUITransform2D
{
public:
	MUITransformBottomOut(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// 逐渐变大出现
class MUITransformZoomIn : public MUITransform2D 
{
public:
	MUITransformZoomIn(int during = 512, bool fade = true);
};

// 逐渐变小消失
class MUITransformZoomOut : public MUITransform2D 
{
public:
	MUITransformZoomOut(int during = 512, bool fade = true);
};

// 淡入
class MUITransformFadeIn : public MUITransform2D
{
public:
	MUITransformFadeIn(int during = 512);
};

// 淡出
class MUITransformFadeOut : public MUITransform2D
{
public:
	MUITransformFadeOut(int during = 512);
};


#endif//_MUI_TRANSFORM_H_
