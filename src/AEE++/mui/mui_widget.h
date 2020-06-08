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
	 * Rect(x, y, width, height) 是相对父窗口左上角(0,0)点的Rect区域
	 */
	MUIWidget(int x, int y, int width, int height, MUIStyle nStyle);

	virtual ~MUIWidget();
	virtual void onPaint(MUIDC* pDC);
	virtual bool onKeyEv(int ev, int code);

	/**
	 * Point(x, y) 是相对屏幕坐标系的x, y
	 */
	virtual bool onPenEv(int ev, int x, int y);

	/**
	 * 滚动条参数
	 */
	virtual MUIScrollInfo* getScrollInfo();
	virtual void setScrollInfo(const MUIScrollInfo* pInfo, int x, int y);
	virtual void setScrollPenOn(bool penOn);
	virtual void setAttribute(const AEEStringRef& strName, const AEEStringRef& strVal);

public:
	/**
	 * 返回当前Widget相对在屏幕左上角(0,0)的Rect
	 */
	MUIRect  getRect() const;
	
	void getRect(MUIRect& rect) const;

	/**
	 * 返回当前Widget相对在屏幕左上角(0,0)的可见Rect
	 */
	bool getVisibleRect(MUIRect* pRect);

	/**
	 * 相对父窗口左上角(0,0)点的Rect区域
	 */
	MUIRect	 getRectAtParent();
	void     setRectAtParent(int x,int y, int width, int height){
		mRectAtParent.x = x;
		mRectAtParent.y = y;
		mRectAtParent.width = width;
		mRectAtParent.height = height;
	}

	/**
	 * 获取滚动条位置
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
