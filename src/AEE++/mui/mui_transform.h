#ifndef _MUI_TRANSFORM_H_
#define _MUI_TRANSFORM_H_

#include "util/AEEStringRef.h"
#include "mui_dc.h"
#include "mui_util.h"

class MUIWindow;
class MUITransform
{
public:
	
	// ���õײ�window,����top window����
	virtual void setBackWindow(MUIWindow* pWnd, int cf) = 0;
	// ���ö���window,����back window����
	virtual void setTopWindow(MUIWindow* pWnd, int cf) = 0;
	// ���ú�ʼ�����л�
	virtual void start() = 0;
	// �л������Ƿ��Ѿ�����
	virtual bool isFinished() = 0;
	// �л����̵Ļ���
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


// ʵ��window������2D�任�л�Ч��
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

// ҳ������ȥ
class MUITransformLeftIn : public MUITransform2D
{
public:
	MUITransformLeftIn(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);

};

// ҳ��������
class MUITransformLeftOut : public MUITransform2D
{
public:
	MUITransformLeftOut(int during = 512, bool fade = true) ;
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// ҳ����ҽ�ȥ
class MUITransformRightIn : public MUITransform2D
{
public:
	MUITransformRightIn(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);

};

// ҳ����ҳ���
class MUITransformRightOut : public MUITransform2D
{
public:
	MUITransformRightOut(int during = 512, bool fade = true) ;
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// ҳ����Ͻ�ȥ
class MUITransformTopIn : public MUITransform2D
{
public:
	MUITransformTopIn(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// ҳ����ϳ���
class MUITransformTopOut : public MUITransform2D
{
public:
	MUITransformTopOut(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// ҳ����½�ȥ
class MUITransformBottomIn : public MUITransform2D
{
public:
	MUITransformBottomIn(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// ҳ����³���
class MUITransformBottomOut : public MUITransform2D
{
public:
	MUITransformBottomOut(int during = 512, bool fade = true);
	virtual void setTopWindow(MUIWindow* pWnd, int cf);
};

// �𽥱�����
class MUITransformZoomIn : public MUITransform2D 
{
public:
	MUITransformZoomIn(int during = 512, bool fade = true);
};

// �𽥱�С��ʧ
class MUITransformZoomOut : public MUITransform2D 
{
public:
	MUITransformZoomOut(int during = 512, bool fade = true);
};

// ����
class MUITransformFadeIn : public MUITransform2D
{
public:
	MUITransformFadeIn(int during = 512);
};

// ����
class MUITransformFadeOut : public MUITransform2D
{
public:
	MUITransformFadeOut(int during = 512);
};


#endif//_MUI_TRANSFORM_H_
