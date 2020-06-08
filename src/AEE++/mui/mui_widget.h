#ifndef _MUI_WIDGET_H_
#define _MUI_WIDGET_H_

#include "util/AEEStringRef.h"
#include "mui_dc.h"
#include "mui_manager.h"
#include "mui_util.h"

class MUIWidget;

class MUIWidgetDraw
{
public:
	virtual ~MUIWidgetDraw();
	virtual void onDraw(MUIWidget* pWidget, MUIDC* pDC) = 0;
};

class MUIScrollDraw
{
public:
	virtual ~MUIScrollDraw();
	virtual void onDraw(MUIDC* pDC, MUIWidget* pWidget) = 0;
};

class MUIWidget
{
public:
	MUIWidget();
	/**
	 * Rect(x, y, width, height) ����Ը��������Ͻ�(0,0)���Rect����
	 */
	MUIWidget(int x, int y, int width, int height, MUIStyle nStyle);

	virtual ~MUIWidget();
	virtual void onPaint(MUIDC* pDC);
	virtual bool onKeyEv(int ev, int code);

	/**
	 * Point(x, y) �������Ļ����ϵ��x, y
	 */
	virtual bool onPenEv(int ev, int x, int y);

	/**
	 * ����������
	 */
	virtual MUIScrollInfo* getScrollInfo();
	virtual void setScrollInfo(const MUIScrollInfo* pInfo, int x, int y);
	virtual void setScrollPenOn(bool penOn);
	virtual void setAttribute(const AEEStringRef& strName, const AEEStringRef& strVal);

public:
	/**
	 * ���ص�ǰWidget�������Ļ���Ͻ�(0,0)��Rect
	 */
	MUIRect  getRect() const;
	
	void getRect(MUIRect& rect) const;

	/**
	 * ���ص�ǰWidget�������Ļ���Ͻ�(0,0)�Ŀɼ�Rect
	 */
	bool getVisibleRect(MUIRect* pRect);

	/**
	 * ��Ը��������Ͻ�(0,0)���Rect����
	 */
	MUIRect	 getRectAtParent();
	void     setRectAtParent(int x,int y, int width, int height){
		mRectAtParent.x = x;
		mRectAtParent.y = y;
		mRectAtParent.width = width;
		mRectAtParent.height = height;
	}

	/**
	 * ��ȡ������λ��
	 */
	MUIPoint getScrollPos();

public:
	void		 setPtr(void* ptr){ mPtr = ptr; }
	void*		 getPtr(){ return mPtr; }
	MUIStyle	 getStyle() { return mStyle; }
	void		 setStyle(MUIStyle nStyle) { mStyle = nStyle; }
	void         setManager(MUIManager* pMgr) { mMgr = pMgr; }
	MUIManager*  getManager() { return mMgr; }
	void		 setParent(MUIWidget* pWidget) { mParent = pWidget; }
	MUIWidget*   getParent() { return mParent; }
	void		 setDraw(MUIWidgetDraw* pDraw);
	unsigned int getID() const{ return mID; }
	void		 setID(unsigned int nId){ mID = nId; }
protected:
	unsigned int    mID;
	MUIManager*		mMgr;
	MUIWidget*		mParent;
	MUIRect			mRectAtParent;
	MUIStyle		mStyle;
	void*			mPtr;
	MUIWidgetDraw*	mDraw;
	MUIScrollInfo*  mScrollInfo;
};

#endif//_MUI_WIDGET_H_
