#ifndef _MUI_TAB_H_
#define _MUI_TAB_H_

#include "mui_container.h"

class MUITab;

struct MUITabItem 
{
	unsigned short	mCaption[16];
	MUIStyle		mStyle;
};
#define MUI_TAB_DRAW_PART_HEADBG 0
#define MUI_TAB_DRAW_PART_ITEM   1
#define MUI_TAB_DRAW_PART_AFTER  2
class MUITabDraw
{
public:
	virtual ~MUITabDraw();
	virtual void OnDraw(MUITab* pTab, int nPart, MUIDC* pDC, int x, int y, int cx, int cy, MUITabItem* pItem) = 0;
};

class MUITab : public MUIContainer
{
public:
	MUITab();
	MUITab(int x, int y, int width, int height, 
		   int tab_width, int tab_height, int tab_max_count, MUIStyle nStyle);
	virtual ~MUITab();
	virtual void onPaint(MUIDC* pDC);
	virtual bool onKeyEv(int ev, int code);
	virtual bool onPenEv(int ev, int x, int y);
	virtual void setAttribute(const AEEStringRef& strName, const AEEStringRef& strVal);
public:
	int			 getSelectItem() { return mSelectIdx; }
	void         setSelectItem(int nIdx, bool bRepaint=true);
	MUITabItem*  setTabItem(int nIdx, const char* strCaption, int nLen, MUIStyle nStyle);
	int          getItemCount();
	MUITabItem*  getItem(int nIdx);

	void         setTabDraw(MUITabDraw* pTabDraw);
	void		 setUpOrDown(bool bUp);
	void		 setLeftOrRight(int attr);
	int			 getInterval(){ return mInterval; }

protected:
	MUIRect		 getTabHeaderRect();
	MUIRect		 getTabItemRect(int nIdx);
	MUIRect		 getTabBodyRect();
	MUIWidget*   getTabBody(int nIdx);
protected:
	MUITabItem* mTabItems;
	int			mCount;
	int			mCapactity;
	int			mSelectIdx;
	int			mTabWidth;
	int			mTabHeight;
	int			mDownIdx;
	int			mInterval;
	bool		mUpOrDown;
	int			mAttr;		//0:¿¿ÓÒ 1:¿¿×ó 2: ¾ÓÖÐ	//Ä¬ÈÏ¿¿ÓÒ
	//bool		mLeftOrRight;
	MUITabDraw*	mTabDraw;
};

#endif//_MUI_TAB_H_
