#ifndef _DUI_LIST_CONTROL_H_
#define _DUI_LIST_CONTROL_H_

#include "dui_control.h"
#include "dui_util.h"
#include "util/AEEList.h"

class DUIListItem
{
public:
	virtual ~DUIListItem(){};
	/**
	 * 获取高度
	 */
	virtual int    GetItemHeight()  = 0;

	/**
	 * 自绘函数
	 */
	virtual void   OnDraw(DUIDC* pDC, int bSelected, int x, int y, int cx, int cy)=0;
};

class DUIListControl : public DUIControl
{
public:
	DUIListControl();
	virtual ~DUIListControl();

	virtual void  SetRect(int x, int y, int cx, int cy);
	virtual void  SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	virtual TSize MeasureSize(int nParentCx, int nParentCy);
	virtual bool  OnKeyEvent(int flag, int ev, int key);
	virtual bool  OnMotionEvent(const DUIMotionEvent* pMotionEv);
	virtual void  SetScrollPos(int x, int y);
	virtual void  GetScrollInfo(DUIScrollInfo& info);
protected:
	virtual void  OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);

	static		void LongTouchCb(int timerID,void* pUser);
public:
	int  GetDataItemCount();
	void SetPageParam(int nTotal, int nPageSize, int nPageIndex);
	DUIListItem* GetDataItem(int nIdx);
	void RemoveDataItem(int idx, bool bRepaint=true);
	void RemoveDataItem(DUIListItem* pItem,bool bRepaint=true);
	void RemoveDataAll();
	void PushBackDataItem(DUIListItem* pItem,  bool bRepaint=true);
	void PushFrontDataItem(DUIListItem* pItem, bool bRepaint=true);
	void PopFront( bool bRepaint=false);
	void SetSelected(int idx){
		mSelectIndex = idx;
	}
	AEEList<DUIListItem*> &GetItems()
	{
		return mDataItems;
	}


private:
	AEEList<DUIListItem*>::iterator GetDataItemInter(int nIdx);
	int HitTest(int posx, int posy);
	TRect GetWindowRect();
	void MakeSelectItemVisible();
protected:
	AEEList<DUIListItem*> mDataItems;
	int					  mScrollY;
	int					  mContentHeight;

	bool				  mUsePager;
	int					  mPageTotal;
	int					  mPageSize;
	int					  mPageIndex;
	
	int					  mSelectIndex;
	int					  mPenDownIndex;
	int					  mMaxDataItem ;

	//常按时间间隔
	int				 mLongTouchIvl;
	int			     mTimerID;
};

#endif//_DUI_LIST_CONTAINER_H_
