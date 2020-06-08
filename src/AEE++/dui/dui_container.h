#ifndef _DUI_CONTAINER_H_
#define _DUI_CONTAINER_H_

#include "util/AEEList.h"
#include "dui_control.h"
#include "layout/dui_layout.h"

#define DUI_LAYOUT_TYPE_HBOX		0
#define DUI_LAYOUT_TYPE_VBOX		1
#define DUI_LAYOUT_TYPE_TAB			2
#define DUI_LAYOUT_TYPE_GRID		3
#define DUI_LAYOUT_TYPE_BORDER		4
#define DUI_LAYOUT_TYPE_RELATIVE	5
#define DUI_LAYOUT_TYPE_VFLOW		6

typedef AEEList<DUIControl*> DUIControls;
class DUIContainer : public DUIControl
{
public:
	DUIContainer();
	virtual ~DUIContainer();
	virtual bool  IsContainer() const;
	virtual void  OnPaint(DUIDC* pDC,int nParentX, int nParentY);
	virtual void  SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	virtual void  SetRect(int x, int y, int cx, int cy);
	virtual TRect GetVirtualRect();
	virtual bool  OnMotionEvent(const DUIMotionEvent* pMotionEv);
	virtual void  SetScrollPos(int x, int y);
	virtual void  GetScrollInfo(DUIScrollInfo& info);
	virtual void  SetLayerId(int layerID);

	int GetScrollPosY()const{return mScrollY;}
public:
	DUIControl*	 GetNextFocusCtrl(bool forward);
	const DUILayout* GetLayout(){return mLayout;}
	int		  GetLayerID(){
		return mLayerId;
	}

	bool SetFocusCtrl(DUIControl* pFocus);
	inline DUIControl *GetFocus()
	{
		return mFocus;
	}
protected:
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);

public:
    DUIControl*	 GetItemAt(int iIndex);
    int			 GetItemIndex(DUIControl* pControl);
    int			 GetCount();
	bool		 Add(DUIControl* pControl);
    bool		 AddAt(DUIControl* pControl, int iIndex);
    bool		 Remove(DUIControl* pControl,bool bDestroy);
    bool		 RemoveAt(int iIndex);
    void		 RemoveAll();
	void		 SetScroll(bool canScroll = true) {mCanScroll = canScroll;}
	bool		 CanScroll() const {return mCanScroll;}
public:
	struct  FocusParam
	{
		DUIContainer* mContainer;
		DUIControl*	  mFocus;
		DUIControl*	  mNext;
	};

	typedef bool (*FindCtrlCallback)(DUIControl*, int);

	static bool NextFocusCallback(DUIControl* pCtrl, int param);
	static bool PrevFocusCallback(DUIControl* pCtrl, int param);
	bool SetScrollbarByFocus(DUIControl* pFocus);
public:
	void RefreshLayout();
	DUIControls& GetItems(){return mItems;}
	DUIControl*  GetChild(const char* szName);
	DUIControl*  GetChild(unsigned int nNameId);
	DUIControl*  FindChild(int param,FindCtrlCallback callback);
	DUIControl*  GetDepthChild(const char* szName);

protected:
	//child controls
	DUIControls			 mItems;

	//layout
	DUILayout*			 mLayout;

	//new layer attribute
	bool				 mUseLayer;
	int					 mLayerId;
	int					 mClrFmt;
	
	//scroll info
	int					 mScrollX;
	int					 mScrollY;
	int					 mContentWidth;
	int					 mContentHeight;

	DUIControl*			 mFocus;
	bool				 mCanScroll;
};

#endif//_DUI_CONTAINER_H_
